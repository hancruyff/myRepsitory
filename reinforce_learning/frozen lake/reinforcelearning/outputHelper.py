import numpy as np
import random
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
        s, done, steps = env.reset(seed=123), False, 0
        state = s[0] # s => (0, {'prob': 1})
        while not done and steps < max_steps:
            state, _, done, _, _ = env.step(pi(state))
            steps += 1
        results.append(state == goal_state)
    return np.sum(results)/len(results)
def mean_return(env, pi, n_episodes=100, max_steps=200):
    random.seed(123); np.random.seed(123)
    results = []
    for _ in range(n_episodes):
        s, done, steps = env.reset(seed=123), False, 0
        state = s[0] # s => (0, {'prob': 1})
        results.append(0.0)
        while not done and steps < max_steps:
            state, reward, done, _, _ = env.step(pi(state))
            results[-1] += reward
            steps += 1
    return np.mean(results)
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
        if (s + 1) % n_cols == 0:
            print("|")