import numpy as np
import random
from itertools import cycle, count, product  # ?아래 2개 통합 필요
#import itertools
from tabulate import tabulate      # 설치 필요, pip install tabulate

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.pylab as pylab

###################### 몬테카를로 제어 추가
## 몬테카를로 제어 알고리즘
def mc_control(env, gamma=1.0, # 환경, 할인율(감마)
    init_alpha=0.5, min_alpha=0.01, alpha_decay_ratio=0.5, # 알파: 초기값, 최종값, 감가비율
    init_epsilon=1.0, min_epsilon=0.1, epsilon_decay_ratio=0.9, # 입실론: 초기값, 최종값, 감가비율
    n_episodes=3000, max_steps=200, # 에피소드 수, 에피소드의 최대 스텝 수
    first_visit=True): # 첫 방문(True), 모든 방문(False) 여부
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
        visited = np.zeros((nS, nA), dtype=np.bool) # 각 행동-상태의 방문 여부 벡터 초기화
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
## 가치 반복 알고리즘
def value_iteration(P, gamma=1.0, theta=1e-10):  # 정책, 환경의 상태, 할인율, 세타(수렴 값)
    V = np.zeros(len(P), dtype=np.float64)
    
    # 수렴할 때까지 반복
    while True:
        Q = np.zeros((len(P), len(P[0])), dtype=np.float64)  # Q-함수를 0으로 초기화
        # 모든 상태들에 대해 반복
        for s in range(len(P)):
            # 현재 상태, 행동에서 가능한 전이에 대해 반복
            for a in range(len(P[s])):
                for prob, next_state, reward, done in P[s][a]:
                    Q[s][a] += prob * (reward + gamma * V[next_state] * (not done))
        # 최대 Q-함수(그리디 정책에 의한 새로운 가치와 이전 가치와의 차이가 세타보다 적으면(수렴하면) 종료
        if np.max(np.abs(V - np.max(Q, axis=1))) < theta:
            break
        V = np.max(Q, axis=1)     # 새로운 가치 함수 갱신
        
    # 최종 정책(최적화된 정책)
    pi = lambda s: {s:a for s, a in enumerate(np.argmax(Q, axis=1))}[s]
    
    return Q, V, pi     # 가치 함수와 정책 리턴
    
## Q-함수 출력 
def print_action_value_function(Q,  optimal_Q=None, action_symbols=('<', '>'), prec=3, title='행동-가치 함수:'):
    vf_types=('',) if optimal_Q is None else ('', '*', 'er')            # 열(행동) 타입: Q, Q*, 오차
    headers = ['s',] + [' '.join(i) for i in list(product(vf_types, action_symbols))]        # 열 헤더 표시
    print(title)
    states = np.arange(len(Q))[..., np.newaxis]      # (state,1) shape
    arr = np.hstack((states, np.round(Q, prec)))   # 행렬 행 결합, (state, len(Q)) shape
    if not (optimal_Q is None):
        arr = np.hstack((arr, np.round(optimal_Q, prec), np.round(optimal_Q-Q, prec)))
                                      # 행렬 행 결합, (state, len(Q+Q*+오차)) shape
    print(tabulate(arr, headers, tablefmt="fancy_grid"))    # 테이블 형식으로 출력

# 정책 성공 확률과 평균 리턴 값, 후회값(regret) 계산
def get_policy_metrics(env, gamma, pi, goal_state, optimal_Q, n_episodes=100, max_steps=200):
    random.seed(123); np.random.seed(123) 
    reached_goal, episode_reward, episode_regret = [], [], []

    # 모든 에피소드에 대해 반복
    for _ in range(n_episodes):
        state, done, steps = env.reset(seed=123), False, 0
        state = state[0]
        episode_reward.append(0.0)
        episode_regret.append(0.0)

        while not done and steps < max_steps:      # 한 에피소드 종료(또는 최대 스텝)까지 반복
            action = pi(state)
            # 후회값은 에피소드 별로 이상적인 행동과 현재 선택 행동 간의 기대 보상치 간의 차이
            regret = np.max(optimal_Q[state]) - optimal_Q[state][action]          
            episode_regret[-1] += regret                         
            state, reward, done, _, _ = env.step(action)
            # 리턴 값 계산
            episode_reward[-1] += (gamma**steps * reward)          
            steps += 1

        reached_goal.append(state == goal_state)
    # 최종 정책 성공 확률과 평균 리턴 값, 후회값(regret) 계산 후 리턴
    results = np.array((np.sum(reached_goal)/len(reached_goal)*100, np.mean(episode_reward), np.mean(episode_regret)))
    return results

###################### 몬테카를로 예측
### 새로 수정/추가된 그래프 및 기타 함수
## 평균 리턴 값 출력
def mean_return(env, gamma, pi, n_episodes=100, max_steps=200):
    random.seed(123); np.random.seed(123)
    results = []
    for _ in range(n_episodes):
        state, done, steps = env.reset(seed=123), False, 0
        state = state[0]               # s => (0, {'prob': 1})
        results.append(0.0)
        while not done and steps < max_steps:
            state, reward, done, _, _ = env.step(pi(state))
            results[-1] += (gamma**steps * reward)   # 할인율 추가
            steps += 1
    return np.mean(results)

## 평균 제곱근 오차(Root Mean Square Error)
def rmse(x, y, dp=4):
    return np.round(np.sqrt(np.mean((x - y)**2)), dp)

## 그래프 크기 파라미터 설정
plt.style.use('fivethirtyeight')
params = {
    'figure.figsize': (15, 8),
    'font.size': 24,
    'legend.fontsize': 20,
    'axes.titlesize': 28,
    'axes.labelsize': 24,
    'xtick.labelsize': 20,
    'ytick.labelsize': 20
}
pylab.rcParams.update(params)
np.set_printoptions(suppress=True)

## 상태-가치함수 그래프 그리기
def plot_value_function(title, V_track, V_true=None, log=False, limit_value=0.05, limit_items=5):
    np.random.seed(123)
    per_col = 25
    linecycler = cycle(["-","--",":","-."])
    legends = []

    valid_values = np.argwhere(V_track[-1] > limit_value).squeeze()
    items_idxs = np.random.choice(valid_values, 
                                  min(len(valid_values), limit_items), 
                                  replace=False)
    # draw the true values first
    if V_true is not None:
        for i, state in enumerate(V_track.T):
            if i not in items_idxs:
                continue
            if state[-1] < limit_value:
                continue

            label = 'v({})'.format(i)
            plt.axhline(y=V_true[i], color='k', linestyle='-', linewidth=1)
            plt.text(int(len(V_track)*1.02), V_true[i]+.01, label)

    # then the estimates
    for i, state in enumerate(V_track.T):
        if i not in items_idxs:
            continue
        if state[-1] < limit_value:
            continue
        line_type = next(linecycler)
        label = 'V({})'.format(i)
        p, = plt.plot(state, line_type, label=label, linewidth=3)
        legends.append(p)
        
    legends.reverse()

    ls = []
    for loc, idx in enumerate(range(0, len(legends), per_col)):
        subset = legends[idx:idx+per_col]
        l = plt.legend(subset, [p.get_label() for p in subset], 
                       loc='center right', bbox_to_anchor=(1.25, 0.5))
        ls.append(l)
    [plt.gca().add_artist(l) for l in ls[:-1]]
    if log: plt.xscale('log')
    plt.title(title)
    plt.ylabel('State-value function')
    plt.xlabel('Episodes (log scale)' if log else 'Episodes')
    plt.show()

## MC 타겟 그래프 그리기
def plot_targets(targets, init_state, title):
    x = range(len(targets[init_state]))
    y = targets[init_state]
    
    label = 'v({})'.format(init_state)
    plt.axhline(y=V_true[init_state], color='k', linestyle='-', linewidth=1)
    plt.text(int(x[-1]*1.02), V_true[init_state]+.01, label)

    plt.scatter(x, y, 
                c=np.array(targets[init_state]),
                cmap=plt.get_cmap('viridis'),
                alpha=0.4)
    plt.title(title)

    plt.ylabel('Target value')
    plt.xlabel('Estimate sequence number')
    plt.show()

#### 동적 계획법의 함수
## 정책 평가 알고리즘
def policy_evaluation(pi, P, gamma=1.0, theta=1e-10):  # 정책, 환경의 상태, 할인율, 세타(수렴 값) 인수    
    prev_V = np.zeros(len(P), dtype=np.float64)       # 이전 가치 함수(k) 를 0으로 초기화
    # 수렴할 때까지 반복
    while True:
        V = np.zeros(len(P), dtype=np.float64)            # 현재 가치 함수(k+1)를 0으로 초기화
         # 모든 상태들에 대해 반복
        for s in range(len(P)):
            # 현재 상태에서 모든 가능한 전이에 대해 반복
            for prob, next_state, reward, done in P[s][pi(s)]:
                  V[s] += prob * (reward + gamma * prev_V[next_state] * (not done))      # 종료 상태인 경우엔 0
        if np.max(np.abs(prev_V - V)) < theta:    # 수렴한 경우 루프 빠져나옴
            break
        prev_V = V.copy()     # 현재 상태를 이전 상태로 복사
    return V          # 계산된 가치함수 값을 리턴

## 정책을 출력
def print_policy(pi, P, action_symbols=('<', 'v', '>', '^'), n_cols=4, title='정책:'):
    print(title)
    arrs = {k:v for k,v in enumerate(action_symbols)}
    for s in range(len(P)):
        a = pi(s)
        print("| ", end="")
        if np.all([done for action in P[s].values() for _, _, _, done in action]):
            print("".rjust(9), end=" ")
        else:
            print(str(s).zfill(2), arrs[a].rjust(6), end=" ")
        if (s + 1) % n_cols == 0: print("|")

## 정책 성공 확률 출력
def probability_success(env, pi, goal_state, n_episodes=100, max_steps=200):
    random.seed(123); np.random.seed(123)
    results = []
    for _ in range(n_episodes):
        state, done, steps = env.reset(seed=123), False, 0
        state = state[0]               # state => (0, {'prob': 1})
        while not done and steps < max_steps:
            state, _, done, _, _ = env.step(pi(state))       
            steps += 1
        results.append(state == goal_state)
    return np.sum(results)/len(results)

## 상태 가치 함수 출력
def print_state_value_function(V, P, n_cols=4, prec=3, title='상태-가치 함수:'):
    print(title)
    for s in range(len(P)):
        v = V[s]
        print("| ", end="")
        if np.all([done for action in P[s].values() for _, _, _, done in action]):
            print("".rjust(9), end=" ")
        else:
            print(str(s).zfill(2), '{}'.format(np.round(v, prec)).rjust(6), end=" ")
        if (s + 1) % n_cols == 0: print("|")
