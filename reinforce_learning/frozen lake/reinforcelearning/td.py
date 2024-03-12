import numpy as np
import random
from tqdm import *
from itertools import cycle, count
import gym
from RLHelper import *
def decay_schedule(init_value, min_value, decay_ratio, max_steps, log_start=-2, log_base=10):
    decay_steps = int(max_steps * decay_ratio) # 감가 비율 사용하여 감가될 스텝(횟수) 게산
    rem_steps = max_steps - decay_steps # 나머지 스텝
# [10-2, .......... 100]의 역순([::-1])
    values = np.logspace(log_start, 0, decay_steps, base=log_base, endpoint=True)[::-1]
# 0과 1 사이의 값으로 정규화
    values = (values - values.min()) / (values.max() - values.min())
# init_value와 min_value 사이 값으로 조정
    values = (init_value - min_value) * values + min_value
# 나머지 스텝은 마지막 min_value로 패딩
    values = np.pad(values, (0, rem_steps), 'edge')
    return values
## 시차 학습 알고리즘
def td(pi, env, gamma=1.0, # 정책, 환경, 감마(할인율)
    init_alpha=0.5, min_alpha=0.01, alpha_decay_ratio=0.5, # 알파(학습계수): 초기값, 최종값, 감가비율
    n_episodes=500): # 에피소드 수
    nS = env.observation_space.n # 상태 수
    V = np.zeros(nS, dtype=np.float64) # 상태-가치함수 초기화
# 오프라인 분석(에피소드 당 가치함수, 타겟(리턴) 값)을 위한 변수 초기화
    V_track = np.zeros((n_episodes, nS), dtype=np.float64)
    targets = {state:[] for state in range(nS)}
# 에피소드마다 지수적 감가되는 알파(학습계수) 값을 한 번에 계산
    alphas = decay_schedule(init_alpha, min_alpha, alpha_decay_ratio, n_episodes)
# 에피소드 당 반복
    for e in tqdm(range(n_episodes), leave=False):
        state, done = env.reset(), False # 상태 초기화
        state = state[0]
# 에피소드의 종료 시까지 반복
        while not done:
            action = pi(state) # 해당 정책의 행동 선택
            next_state, reward, done, _, _ = env.step(action) # 행동 수행 후 다음 상태, 보상 수집
# 가치함수 갱신
            td_target = reward + gamma * V[next_state] * (not done) # TD 타겟 계산
            targets[state].append(td_target)
            td_error = td_target - V[state] # TD 에러 계산
            V[state] = V[state] + alphas[e] * td_error # 상태 갱신
            state = next_state
        V_track[e] = V
    return V, V_track, targets # 가치함수 리턴
## 프로즌 레이크 환경 생성
env = gym.make('FrozenLake-v1')
# 파라미터 초기화
goal_state = 15
gamma = 0.99
n_episodes = 2500
P = env.env.P # 상태
## 샘플 정책 생성
LEFT, DOWN, RIGHT, UP = range(4)
pi = lambda s: {
0:LEFT, 1:UP, 2:UP, 3:UP,
4:LEFT, 5:LEFT, 6:LEFT, 7:LEFT,
8:UP, 9:DOWN, 10:LEFT, 11:LEFT,
12:LEFT, 13:RIGHT, 14:DOWN, 15:LEFT
}[s]
## 참 상태-가치함수 계산: 동적 계획법의 정책 평가
# 동적 계획법의 정책 평가 수행
V_true = policy_evaluation(pi, P, gamma=gamma)
# 가치함수, 정책, 성공 확률 출력
print(">> 참 상태-가치함수: 동적 계획법 정책 평가")
print_state_value_function(V_true, P)
print()
print_policy(pi, P)
print('성공확률 {:.2f}%. 평균리턴값 {:.4f}.'.format(
probability_success(env, pi, goal_state=goal_state), 
mean_return(env, gamma, pi)))

## 시차 학습
# 각기 다른 시드 값으로 5번 수행 후의 상태-가치함수 평균 값
SEEDS = (12, 34, 56, 78, 90) 
V_tds, V_track_tds = [], [] # 5번 수행 상태-가치함수 임시 저장
# 5개 시드에 대해 FVMC 반복 수행
for seed in tqdm(SEEDS, desc='All seeds', leave=True):
    random.seed(seed); np.random.seed(seed) ; env.reset(seed=seed)
# 시차 학습 수행
    V_td, V_track_td, targets_td = td(pi, env, gamma=gamma, n_episodes=n_episodes)
# 상태-가치함수 저장
    V_tds.append(V_td) ; V_track_tds.append(V_track_td)
# 5개의 상태-가치함수 평균 값 계산
V_td, V_track_td = np.mean(V_tds, axis=0), np.mean(V_track_tds, axis=0)
del V_tds ; del V_track_tds
# 가치함수, 가치함수 에러, 평균 제곱근 오차 출력
print(">> 시차 학습")
print_state_value_function(V_td, P)
print()
print_state_value_function(V_td - V_true, P, title='State-value function errors:')
print('RMSE:', rmse(V_td, V_true))
## 에피소드 진행에 따른 FVMC 상태-가치함수 값 추정 그래프, 5개 상태 의의 선택
# 에피소드가 진행함에 따라 참의 값으로 수렴
plot_value_function('TD estimates through time vs. true values', V_track_td, V_true, log=False)
# x축을 로그 스케일로 변환한 그래프
plot_value_function('TD estimates through time vs. true values (log scale)', V_track_td, V_true, log=True)