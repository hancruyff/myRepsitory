## 정책 평가 알고리즘
def policy_evaluation(pi, P, gamma=1.0, theta=1e-10): # 정책, 환경의 상태, 할인율, 세타(수렴 값)
    prev_V = np.zeros(len(P), dtype=np.float64) # 이전 가치 함수(k) 를 0으로 초기화
# 수렴할 때까지 반복
    while True:
        V = np.zeros(len(P), dtype=np.float64) # 현재 가치 함수(k+1)를 0으로 초기화
# 모든 상태들에 대해 반복
        for s in range(len(P)):
# 현재 상태에서 가능한 전이에 대해 반복
            for prob, next_state, reward, done in P[s][pi(s)]:
                V[s] += prob * (reward + gamma * prev_V[next_state] * (not done)) # 종료 상태(done)인 경우엔 0
        if np.max(np.abs(prev_V - V)) < theta: # 수렴한 경우 루프 빠져나옴
            break
        prev_V = V.copy() # 현재 상태를 이전 상태로 복사
    return V # 계산된 가치함수 값을 리턴
## 정책 개선 알고라즘
def policy_improvement(V, P, gamma=1.0): # 상태-가치함수, 환경의 상태, 할인율 인수
    Q = np.zeros((len(P), len(P[0])), dtype=np.float64) # 상태-가치 함수를 0으로 초기화
# 모든 상태들에 반복
    for s in range(len(P)):
# 현재 상태에서 모든 가능한 행동에 대해 반복
        for a in range(len(P[s])):
# 현재 상태, 행동에서 가능한 전이에 대해 반복
            for prob, next_state, reward, done in P[s][a]:
                Q[s][a] += prob * (reward + gamma * V[next_state] * (not done)) # 종료 상태(done)인 경우엔 0
# 그리디 개선으로 정책을 개선
    new_pi = lambda s: {s:a for s, a in enumerate(np.argmax(Q, axis=1))}[s]
    return new_pi # 개선된 새로운 정책을 리턴
## 가치 반복 알고리즘
def value_iteration(P, gamma=1.0, theta=1e-10): # 정책, 환경의 상태, 할인율, 세타(수렴 값)
    V = np.zeros(len(P), dtype=np.float64) 
# 수렴할 때까지 반복
    while True:
        Q = np.zeros((len(P), len(P[0])), dtype=np.float64) # Q-함수를 0으로 초기화
# 모든 상태들에 대해 반복
        for s in range(len(P)):
# 현재 상태, 행동에서 가능한 전이에 대해 반복
            for a in range(len(P[s])):
                for prob, next_state, reward, done in P[s][a]:
                    Q[s][a] += prob * (reward + gamma * V[next_state] * (not done))
# 최대 Q-함수(그리디 정책에 의한 새로운 가치와 이전 가치와의 차이가 세타보다 적으면(수렴하면) 종료
        if np.max(np.abs(V - np.max(Q, axis=1))) < theta:
            break
        V = np.max(Q, axis=1) # 새로운 가치 함수 갱신
# 최종 정책(최적화된 정책)
    pi = lambda s: {s:a for s, a in enumerate(np.argmax(Q, axis=1))}[s]
    return V, pi # 가치 함수와 정책 리턴
import gym
from outputHelper import *
## 프로즌 레이크 환경 생성
env = gym.make('FrozenLake-v1')
# 상태 저장
P = env.env.P
init_state = env.reset(seed=123) # 환경 초기화
goal_state = 15 # 목표 상태
## 임의의 정책 생성
LEFT, DOWN, RIGHT, UP = range(4)
random_pi = lambda s: {
0:RIGHT, 1:LEFT, 2:DOWN, 3:UP,
4:LEFT, 5:LEFT, 6:RIGHT, 7:LEFT,
8:UP, 9:DOWN, 10:UP, 11:LEFT,
12:LEFT, 13:RIGHT, 14:DOWN, 15:LEFT
}[s]
adversarial_pi = lambda s: {
0:LEFT, 1:UP, 2:UP, 3:UP,
4:LEFT, 5:LEFT, 6:UP, 7:LEFT,
8:UP, 9:DOWN, 10:LEFT, 11:LEFT,
12:LEFT, 13:RIGHT, 14:RIGHT, 15:LEFT
}[s]
print_policy(adversarial_pi, P)
print('성공확률 {:.2f}%. 평균리턴값 {:.4f}.'.format(
probability_success(env, adversarial_pi, goal_state=goal_state)*100, 
mean_return(env, adversarial_pi)))
V = np.zeros(len(P), dtype=np.float64) # 가치 함수를 0으로 초기화
n = 0
pi = adversarial_pi

# 정책, 성공 확률, 평균 리턴 값 출력
#print_policy(random_pi, P)
#print('성공확률 {:.2f}%. 평균리턴값 {:.4f}.'.format(probability_success(env, random_pi,goal_state=goal_state)*100,mean_return(env, random_pi)))
## 정책 평가 수행
#V = policy_evaluation(random_pi, P, gamma=0.99)
# 상태-가치 함수 출력
#print_state_value_function(V, P, prec=4)
## 정책 개선 수행
#improved_pi = policy_improvement(V, P)
# 개선된 정책, 성공 확률, 평균 리턴 값 출력
#print_policy(improved_pi, P)
#print('성공확률 {:.2f}%. 평균리턴값 {:.4f}.'.format(
#probability_success(env, improved_pi, goal_state=goal_state)*100, mean_return(env, improved_pi)))
# 더 이상 정책 개선이 안될 때까지 반복
## 가치 반복 수행
V_best_v, pi_best_v = value_iteration(P, gamma=0.99)
print('최적화된 정책과 상태-가치 함수 (VI):')
print_policy(pi_best_v, P)
print('성공확률 {:.2f}%. 평균리턴값 {:.4f}.'.format(
probability_success(env, pi_best_v, goal_state=goal_state)*100, 
mean_return(env, pi_best_v)))
print()
print_state_value_function(V_best_v, P, prec=4)
