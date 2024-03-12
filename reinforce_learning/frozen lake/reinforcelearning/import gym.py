import gym
env = gym.make('FrozenLake-v1', render_mode='human')
env.env.P
goal_steps = 500
# 에피소드 반복
for e in range(10):
    state = env.reset() # 환경 초기화
# 최대 에피소드까지 반복
    for t in range(goal_steps):
# 임의의 액션 지정
        action = env.action_space.sample()
# 액션을 수행하고 상태, 보상, 종료 여부, 부가정보 리턴
        state, reward, done, truncated, info = env.step(action)
        print(state, ',', reward, ',', done, ',’, truncated, ','', info)
# 종료된 경우 타임 스텝 루프 빠져나와 반복
        if done: 
            print("Episode finished after {} timesteps".format(t+1))
            break
# 격자 디스플레이
        env.render()