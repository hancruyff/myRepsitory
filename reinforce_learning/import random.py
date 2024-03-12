import random
STD = {
    'FB': {
        'C1': 1,
        'C2': 0,
        'C3':0,
        'PB':0,
        'PS':0,
        'SP':0,
        'FB': 9,
        'R':-1,
        'E' :False     
    },
    'C1': {
        'C1':0,
        'C2': 5,
        'C3':0,
        'PB':0,
        'PS':0,
        'SP':0,
        'FB': 10,
        'R':-2,
        'E' :False
    },
    'C2': {
        'C1':0,
        'C2': 0,
        'C3': 8,
        'PB':0,
        'PS':0,
        'SP': 10, 
        'FB': 0, 
        'R':-2, 
        'E' :False  
    },
    'C3': {
        'C1':0,
        'C2': 0,
        'C3': 0,
        'PB':4,
        'PS':10,
        'SP': 0,
        'FB': 0,
        'R':-2,
        'E' :False
    },
    'PS': {
        'C1':0,
        'C2': 0,
        'C3': 0,
        'PB':0,
        'PS':0,
        'SP': 10,
        'FB': 0,
        'R':10,
        'E' :False
    },
    'PB': {
        'C1':2,
        'C2':6,
        'C3':10,
        'PB':0,
        'PS':0,
        'SP': 0,
        'FB': 0,
        'R':1,
        'E' :False
    },
    'SP':{'C1':0,
        'C2': 0,
        'C3': 0,
        'PB':0,
        'PS':0,
        'SP': 0,
        'FB': 0,
        'R':0,
        'E':True}
}
keys=['C1','C2','C3','PB','PS','SP','FB']

goal_step=500
episode = []
for e in range(10):
    s='C1'
    episode.append(s)
    R=-2
    print('----------------')
    for t in range(goal_step):
        r=random.randrange(1,11)
        for b in keys:
            a=STD[s][b]
            if r<=a:
                s=b
                #print(s,'E: ',STD[s]["E"])
                episode.append(s)
                R=R+STD[s]['R']
                break
            if STD[s]["E"]==True:
                R=R+STD[s]['R']
                break
    print(*episode)
    print("Episode finished after {0} timesteps reward {1}".format(len(episode),R))
    episode.clear()
            



