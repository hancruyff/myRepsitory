
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
## 전체 경로의 경험 튜플을 입실론 그리디 탐색으로 한 번에 생성
def generate_trajectory(select_action, Q, epsilon, env, max_steps=200):
    done, trajectory = False, []
    while not done:
        state = env.reset()
        state = state[0]
        for t in count():
            action = select_action(state, Q, epsilon) # 입실론-그리디 탐색 기반 행동을 선택
            next_state, reward, done, _, _ = env.step(action) # 행동 수행 후 다음 상태, 보상 수집
            experience = (state, action, reward, next_state, done) # 경험 튜플 저장
            trajectory.append(experience)
            if done: # 에피소드 최종 상태 도달 시 종료
                break
            if t >= max_steps - 1: # 최대 스텝 수 초과 시 초기화 후 반복
                trajectory = []
                break
        state = next_state
    return np.array(trajectory, np.object_)
import numpy as np
import random
from tqdm import *
from itertools import cycle, count
import gym
from RLHelper import * # 몬테카를로 헬퍼 모듈
## 몬테카를로 제어 알고리즘
def mc_control(env, gamma=1.0, # 환경, 할인율(감마)
init_alpha=0.5, min_alpha=0.01, alpha_decay_ratio=0.5, # 알파: 초기값, 최종값, 감가비율
init_epsilon=1.0, min_epsilon=0.1, epsilon_decay_ratio=0.9, # 입실론: 초기값, 최종값, 감가비율
n_episodes=3000, max_steps=200, # 에피소드 수, 에피소드의 최대 스텝 수
first_visit=False): # 첫 방문(True), 모든 방문(False) 여부
    nS, nA = env.observation_space.n, env.action_space.n # 상태, 행동 수
# 할인율(감마)을 한 번에 계산, [r0, r1, r2, ....., rmax_steps-1]
    discounts = np.logspace(0, max_steps, num=max_steps, base=gamma, endpoint=False)
# 에피소드마다 지수적 감가되는 알파(학습계수) 값을 한 번에 계산
    alphas = decay_schedule(init_alpha, min_alpha, alpha_decay_ratio, n_episodes)
# 에피소드마다 지수적 감가되는 입실론 값을 한 번에 계산
    epsilons = decay_schedule(init_epsilon, min_epsilon, epsilon_decay_ratio, n_episodes)
# Q-함수 초기화
    Q = np.zeros((nS, nA), dtype=np.float64)
# 오프라인 분석(에피소드 당 Q-함수,정책 값)을 위한 변수 초기화
    Q_track = np.zeros((n_episodes, nS, nA), dtype=np.float64)
    pi_track = []
# 입실론-그리디 탐색 기반 행동을 선택하는 람다 함수 정의
    select_action = lambda state, Q, epsilon: np.argmax(Q[state]) \
    if np.random.random() > epsilon \
    else np.random.randint(len(Q[state])) # 탐색
# 에피소드 당 반복
    for e in tqdm(range(n_episodes), leave=False):
# 전체 경로의 경험 튜플을 입실론 그리디 탐색으로 한 번에 생성
        trajectory = generate_trajectory(select_action, Q, epsilons[e], env, max_steps)
        visited = np.zeros((nS, nA), dtype=np.bool_) # 각 행동-상태의 방문 여부 벡터 초기화
# 에피소드의 각 상태 당 반복
        for t, (state, action, reward, _, _) in enumerate(trajectory):
            if visited[state][action] and first_visit: # 이미 방문한 상태이고 FVMC 이면 다음 상태로 이동
                continue
            visited[state][action] = True # 방문 상태로 표시
            n_steps = len(trajectory[t:]) # 최종 상태까지의 스텝 수
# Q-함수 갱신
            G = np.sum(discounts[:n_steps] * trajectory[t:, 2])
            Q[state][action] = Q[state][action] + alphas[e] * (G - Q[state][action])
        Q_track[e] = Q # 에피소드의 Q-함수 저장
        pi_track.append(np.argmax(Q, axis=1)) # 에피소드의 정책 저장
    V = np.max(Q, axis=1) # 개선된 최종 상태-가치함수
    pi = lambda s: {s:a for s, a in enumerate(np.argmax(Q, axis=1))}[s] # 개선된 최적의 정책
    return Q, V, pi, Q_track, pi_track
## 프로즌 레이크 환경 생성
env = gym.make('FrozenLake-v1')
# 파라미터 초기화
init_state = env.reset()
goal_state = 15
gamma = 0.99
n_episodes = 10000
P = env.env.P # 상태
# 가치함수 열 수, 가치 /Q 함수 출력 소수점 이하 자릿수
n_cols, svf_prec, err_prec, avf_prec=4, 4, 2, 3
action_symbols=('<', 'v', '>', '^')
# 그래프 출력 시 아이템 수, 최소값, 에피소드 수
limit_items, limit_value = 5, 0.0
cu_limit_items, cu_limit_value, cu_episodes = 10, 0.01, 2000
## 최적의 가치함수와 정책
# 동적 계획법의 가치 반복 수행
optimal_Q, optimal_V, optimal_pi = value_iteration(P, gamma=gamma)
# 최적의 가치/Q 함수, 정책, 성공 확률 출력
print(">> 최적의 가치/Q 함수, 정책: 동적 계획법 가치 반복")
print_state_value_function(optimal_V, P, n_cols=n_cols, prec=svf_prec, title='Optimal state-value function:')
print()
print_action_value_function(optimal_Q, None, action_symbols=action_symbols, prec=avf_prec,
title='Optimal action-value function:')
print()
print_policy(optimal_pi, P, action_symbols=action_symbols, n_cols=n_cols)
# 정책 성공 확률과 평균 리턴 값, 후회값(regret)
# 후회값은 에피소드 별로 이상적인 행동과 현재 선택 행동 간의 기대 보상치 간의 차이, 작을 수록 좋은 성능
success_rate_op, mean_return_op, mean_regret_op = get_policy_metrics(env, gamma=gamma, pi=optimal_pi,
goal_state=goal_state, optimal_Q=optimal_Q)
print('Reaches goal {:.2f}%. Obtains an average return of {:.4f}. Regret of {:.4f}'.format(success_rate_op,
mean_return_op, mean_regret_op))
## 첫 방문 몬테카를로 제어 (First Visit Monte Carlo Control)
# 각기 다른 시드 값으로 5번 수행 후의 Q/가치함수 평균 값 사용
SEEDS = (12, 34, 56, 78, 90)
Q_mcs, V_mcs, Q_track_mcs = [], [], [] # 5번 수행 Q/가치 함수 임시 저장
# 5개 시드에 대해 FVMC 반복 수행
for seed in tqdm(SEEDS, desc='All seeds', leave=True):
    random.seed(seed); np.random.seed(seed) ; env.reset(seed=123)
# 몬테카를로 제어 수행
    Q_mc, V_mc, pi_mc, Q_track_mc, pi_track_mc = mc_control(env, gamma=gamma, n_episodes=n_episodes)
# Q/가치 함수 임시 저장
    Q_mcs.append(Q_mc) ; V_mcs.append(V_mc) ; Q_track_mcs.append(Q_track_mc)
# 5개의 Q/가치 함수 평균 값 계산
Q_mc, V_mc, Q_track_mc = np.mean(Q_mcs, axis=0), np.mean(V_mcs, axis=0), np.mean(Q_track_mcs, axis=0)
del Q_mcs ; del V_mcs ; del Q_track_mcs # 임시 변수 제거
# FVMC 가치함수, 최적화된 가치함수, 오차, RMSE 출력
print_state_value_function(V_mc, P, n_cols=n_cols, prec=svf_prec, title='State-value function found by FVMC:')
print_state_value_function(optimal_V, P, n_cols=n_cols, prec=svf_prec, title='Optimal state-value function:')
print_state_value_function(V_mc - optimal_V, P, n_cols=n_cols, prec=err_prec, title='State-value function errors:')
print('State-value function RMSE: {}'.format(rmse(V_mc, optimal_V)))
print()
# FVMC Q 함수, 최적화된 Q 함수, 오차, RMSE, 정책, 평균 리턴 출력
print_action_value_function(Q_mc, optimal_Q, action_symbols=action_symbols, prec=avf_prec,
title='FVMC action-value function:')
print('Action-value function RMSE: {}'.format(rmse(Q_mc, optimal_Q)))
print()
print_policy(pi_mc, P, action_symbols=action_symbols, n_cols=n_cols)
success_rate_mc, mean_return_mc, mean_regret_mc = get_policy_metrics(env, gamma=gamma, pi=pi_mc,
goal_state=goal_state, optimal_Q=optimal_Q)
print('Reaches goal {:.2f}%. Obtains an average return of {:.4f}. Regret of {:.4f}'.format(
success_rate_mc, mean_return_mc, mean_regret_mc))
## 에피소드 진행에 따른 FVMC 상태-가치함수 값 추정 그래프, 5개 상태 임의의 선택
plot_value_function('FVMC estimates through time vs. true values', np.max(Q_track_mc, axis=2), optimal_V,limit_items=limit_items, limit_value=limit_value, log=False)
plot_value_function('FVMC estimates through time vs. true values (log scale)', np.max(Q_track_mc, axis=2), optimal_V,limit_items=limit_items, limit_value=limit_value, log=True) # 로그 스케일
plot_value_function('FVMC estimates through time (close up)', np.max(Q_track_mc, axis=2)[:cu_episodes], None,limit_items=cu_limit_items, limit_value=cu_limit_value, log=False) # 0~2000 에피소드까지 확대