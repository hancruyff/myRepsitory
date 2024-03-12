
import numpy as np
import random
from tqdm import *
from itertools import cycle, count
import gym
from RLHelper import * # 헬퍼 모듈
## 몬테카를로 예측 알고리즘
def mc_prediction(pi, env, gamma=1.0, # 정책, 환경, 감마(할인율)
    init_alpha=0.5, min_alpha=0.01, alpha_decay_ratio=0.5, # 알파(학습계수): 초기값, 최종값, 감가비율
    n_episodes=500, max_steps=200, # 에피소드 수, 에피소드의 최대 스텝 수
    first_visit=False): # 첫 방문(True), 모든 방문(False) 여부
    nS = env.observation_space.n # 상태 수
# 할인율(감마)을 한 번에 계산, [r0, r1, r2, ....., rmax_steps-1]
# 파라미터: 시작값, 최종값, 샘플수, 로그 베이스, 최종값 포함 여부
    discounts = np.logspace(0, max_steps, num=max_steps, base=gamma, endpoint=False)
# 에피소드마다 지수적 감가되는 알파(학습계수) 값을 한 번에 계산
    alphas = decay_schedule(init_alpha, min_alpha, alpha_decay_ratio, n_episodes)
    V = np.zeros(nS, dtype=np.float64) # 상태-가치함수 초기화
# 오프라인 분석(에피소드 당 가치함수, 타겟(리턴) 값)을 위한 변수 초기화
    V_track = np.zeros((n_episodes, nS), dtype=np.float64)
    targets = {state:[] for state in range(nS)}
    # 에피소드 당 반복, tqdm 라이브러리 사용하여 진행률(progress bar) 표시
# 잔상 표시하지 않음(leave=False), pip install tqdm으로 설치
    for e in tqdm(range(n_episodes), leave=False):
# 전체 경로의 경험 튜플을 한 번에 생성
        trajectory = generate_trajectory(pi, env, max_steps)
        visited = np.zeros(nS, dtype=np.bool_) # 각 상태의 방문 여부 벡터 초기화
# 에피소드의 각 상태 당 반복
        for t, (state, _, reward, _, _) in enumerate(trajectory):
            if visited[state] and first_visit: # 이미 방문한 상태이고 FVMC 이면 다음 상태로 이동
                continue
            visited[state] = True # 방문 상태로 표시
            n_steps = len(trajectory[t:]) # 최종 상태까지의 스텝 수
# 리턴 값 계산
            G = np.sum(discounts[:n_steps] * trajectory[t:, 2])
            targets[state].append(G)
# MC 에러 계산
            mc_error = G - V[state]
# 상태 갱신
            V[state] = V[state] + alphas[e] * mc_error
        V_track[e] = V
    return V.copy(), V_track, targets # 상태-가치함수 리턴(deep copy)
## 에피소드마다 지수적 감가되는 알파(학습계수) 값 한 번에 계산
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
## 에피소드의 전체 경로의 경험 튜플을 한 번에 생성
def generate_trajectory(pi, env, max_steps=200):
    done, trajectory = False, []
    while not done:
        state = env.reset()
        state = state[0]
        for t in count():
            action = pi(state) # 해당 정책의 행동 선택
            next_state, reward, done, _, _ = env.step(action) # 행동 수행 후 다음 상태, 보상 수집
# 경험 튜플 저장
            experience = (state, action, reward, next_state, done)
            trajectory.append(experience)
            if done: # 에피소드 최종 상태 도달 시 종료
                break
            if t >= max_steps - 1: # 최대 스텝 수 초과 시 초기화 후 반복
                trajectory = []
                break
            state = next_state
    return np.array(trajectory, np.object_)
## 프로즌 레이크 환경 생성
env = gym.make('FrozenLake-v1')
# 파라미터 초기화
init_state = env.reset()
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
## 첫 방문 몬테카를로 예측 (First Visit Monte Carlo Prediction)
# 각기 다른 시드 값으로 5번 수행 후의 상태-가치함수 평균 값
SEEDS = (12, 34, 56, 78, 90)
V_fvmcs, V_track_fvmcs = [], [] # 5번 수행 상태-가치함수 임시 저장
# 5개 시드에 대해 FVMC 반복 수행
for seed in tqdm(SEEDS, desc='All seeds', leave=True):
    random.seed(seed); np.random.seed(seed); env.reset(seed=seed)
# 몬테카를로 예측 수행
    V_fvmc, V_track_fvmc, targets_fvmc = mc_prediction(pi, env, gamma=gamma, n_episodes=n_episodes)
# 상태-가치함수 저장
    V_fvmcs.append(V_fvmc) ; V_track_fvmcs.append(V_track_fvmc)
# 5개의 상태-가치함수 평균 값 계산
V_fvmc, V_track_fvmc = np.mean(V_fvmcs, axis=0), np.mean(V_track_fvmcs, axis=0)
del V_fvmcs ; del V_track_fvmcs # 임시 변수 제거
# 가치함수, 가치함수 에러, 평균 제곱근 오차 출력
print(">> 첫 방문 몬테카를로 예측 (FVMC)")
print_state_value_function(V_fvmc, P)
print()
print_state_value_function(V_fvmc - V_true, P, title='State-value function errors:')
print('RMSE:', rmse(V_fvmc, V_true))
## 에피소드 진행에 따른 FVMC 상태-가치함수 값 추정 그래프, 5개 상태 임의의 선택
# 에피소드가 진행함에 따라 참의 값으로 수렴
plot_value_function('FVMC estimates through time vs. true values', V_track_fvmc, V_true, log=False)
# x축을 로그 스케일로 변환한 그래프
plot_value_function('FVMC estimates through time vs. true values (log scale)', V_track_fvmc, V_true, log=True)
