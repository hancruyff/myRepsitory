import numpy as np
from tqdm import *
from collections import defaultdict
# 그리드 월드 환경
from GridSQ_environment import Env, WIDTH, HEIGHT
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
## 살사(Sarsa) 알고리즘
def sarsa(env, gamma=1.0, # 환경, 할인율(감마)
    init_alpha=0.5, min_alpha=0.01, alpha_decay_ratio=0.5, # 알파: 초기값, 최종값, 감가비율
    init_epsilon=1.0, min_epsilon=0.1, epsilon_decay_ratio=0.9, # 입실론: 초기값, 최종값, 감가비율
    n_episodes=3000): # 에피소드 수
    nS = WIDTH*HEIGHT # 그리드 월드 총 상태 수, 5*5
    nA = 4 # 행동 수, [‘상’,‘ 하’,‘좌’,‘우’] -> [0, 1, 2, 3] # 알파, 입실론 초기화
    alpha = init_alpha
    epsilon = init_epsilon
# Q-함수, 정책 초기화
    Q = np.zeros((nS, nA), dtype=np.float64) 
# 그래픽 디스플레이용 큐 함수 테이블 생성, 그리드 환경과 호환 딕셔너리, 키: 상태의 좌표
    q_table = defaultdict(lambda: [0.0, 0.0, 0.0, 0.0])
# 입실론-그리디 탐색 기반 행동을 선택하는 람다 함수 정의
    select_action = lambda state, Q, eps: np.argmax(Q[state]) \
    if np.random.random() > eps \
    else np.random.randint(len(Q[state])) # 탐색
    alphas = decay_schedule(init_alpha, min_alpha, alpha_decay_ratio, n_episodes)
# 목표지점/장애물 도달 추적 리스트 초기화
    exit_track = []
# 에피소드 당 반복
    for e in tqdm(range(n_episodes), desc='All Episodes'):
        init_xy, done = env.reset(), False # 상태 초기화
        state = state_num(init_xy) # 환경 상태(그리드 위치) 에서 상태 번호로 변환
        action = select_action(state, Q, epsilon) # ε-그리디 탐색 기반 행동을 선택
# 에피소드의 종료 시까지 반복
        while not done:
            next_xy, reward, done = env.step(action) # 행동 수행 후 다음 상태, 보상 수집
            next_state = state_num(next_xy) 
            next_action = select_action(next_state, Q, epsilon) # ε-그리디 탐색 기반 다음 상태 행동을 선택
            td_target = reward + gamma * Q[next_state][next_action] * (not done) # Q-함수 기반 TD 타겟 계산
            td_error = td_target - Q[state][action] # Q-함수 기반 TD 에러 계산
            Q[state][action] = Q[state][action] + alphas[e] * td_error # Q-함수(행동-가치 함수) 갱신
# 그래픽용 Q 함수 갱신, 컴퓨터 화면 기준, x는 그리드의 열, y는 그리드의 행
            x = state % WIDTH; y = state // WIDTH;state_xy = [x, y] 
            q_table[str(state_xy)][action] = Q[state][action] 

            state, action = next_state, next_action # 상태 전이
# 모든 큐 함수 값을 화면에 표시
        env.print_value_all(q_table)
# 에피소드 당 알파, 입실론 감가
        if (alpha > min_alpha):
            alpha *= alpha_decay_ratio 
        if (epsilon > min_epsilon):
            epsilon *= epsilon_decay_ratio
# 에피소드 당 종료 추적 리스트 추가
        if (reward == 100): # 목표 지점 도달
            exit_track.append(1)
        else: # 장애물 충돌 , -100
            exit_track.append(0)
# 목표지점 도달, 장애물 도달 횟수, 비율 출력
    goal_num = exit_track.count(1)
    goal_rate = goal_num/len(exit_track)
    fail_num = exit_track.count(0)
    fail_rate = fail_num/len(exit_track)
    print("목표지점 도달 {:3d} {:5.2f}%, 장애물 도달 {:3d} {:5.2f}%".format(
    goal_num, goal_rate, fail_num, fail_rate))
    return exit_track
## 그리드 월드 환경 생성
env = Env('Sarsa')
# 환경의 상태인 그리드 좌표 [x(열), y(행)]에서 상태 번호 계산 람다 함수
state_num = lambda s: s[0]+HEIGHT*s[1] 
gamma = 0.99 # 할인율
max_episodes = 300 # 최대 에피소드 수
## 살사 학습 수행
done_track = sarsa(env, gamma=gamma, n_episodes=max_episodes)
## 에피소드 당 목표지점 도달 (1), 장애물 도달 (0) 그래프
import matplotlib.pyplot as plt
import matplotlib.pylab as pylab
# matplotlib 파라미터 설정
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
plt.plot(done_track)
plt.title('Sarsa: Grid World')
plt.xlabel('episode')
plt.ylabel('fail, goal') 
plt.show
