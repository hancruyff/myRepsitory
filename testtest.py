import random
import time
import math
import os.path

import numpy as np
import pandas as pd

import matplotlib.pyplot as plt
from pysc2.agents import base_agent
from pysc2.env import sc2_env, run_loop
from pysc2.lib import actions, features, units
from absl import app

import torch
from torch.utils.tensorboard import SummaryWriter

from skdrl.pytorch.model.mlp import NaiveMultiLayerPerceptron
from skdrl.pytorch.model.naivedqn import NaiveDQN
from skdrl.pytorch.util.train_util import EMAMeter

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
writer = SummaryWriter()
Rwriter=SummaryWriter()
class TerranAgentWithRawActsAndRawObs(base_agent.BaseAgent):
    actions = ("do_nothing",
               "harvest_minerals",
               "build_supply_depot",
               "build_barracks",
               "train_marine",
               "attack")
    x=25
    y=25
    def get_my_units_by_type(self, obs, unit_type):
        return [unit for unit in obs.observation.raw_units
                if unit.unit_type == unit_type
                and unit.alliance == features.PlayerRelative.SELF]

    def get_enemy_units_by_type(self, obs, unit_type):
        return [unit for unit in obs.observation.raw_units
                if unit.unit_type == unit_type
                and unit.alliance == features.PlayerRelative.ENEMY]

    def get_my_completed_units_by_type(self, obs, unit_type):
        return [unit for unit in obs.observation.raw_units
                if unit.unit_type == unit_type
                and unit.build_progress == 100
                and unit.alliance == features.PlayerRelative.SELF]

    def get_enemy_completed_units_by_type(self, obs, unit_type):
        return [unit for unit in obs.observation.raw_units
                if unit.unit_type == unit_type
                and unit.build_progress == 100
                and unit.alliance == features.PlayerRelative.ENEMY]

    def get_distances(self, obs, units, xy):
        units_xy = [(unit.x, unit.y) for unit in units]
        return np.linalg.norm(np.array(units_xy) - np.array(xy), axis=1)

    def step(self, obs):
        super(TerranAgentWithRawActsAndRawObs, self).step(obs)
        if obs.first():
            command_center = self.get_my_units_by_type(
                obs, units.Terran.CommandCenter)[0]
            self.base_top_left = (command_center.x < 32)

    def do_nothing(self, obs):
        return actions.RAW_FUNCTIONS.no_op()

    def harvest_minerals(self, obs):
        scvs = self.get_my_units_by_type(obs, units.Terran.SCV)
        idle_scvs = [scv for scv in scvs if scv.order_length == 0]
        if len(idle_scvs) > 0:
            mineral_patches = [unit for unit in obs.observation.raw_units
                               if unit.unit_type in [
                                   units.Neutral.BattleStationMineralField,
                                   units.Neutral.BattleStationMineralField750,
                                   units.Neutral.LabMineralField,
                                   units.Neutral.LabMineralField750,
                                   units.Neutral.MineralField,
                                   units.Neutral.MineralField750,
                                   units.Neutral.PurifierMineralField,
                                   units.Neutral.PurifierMineralField750,
                                   units.Neutral.PurifierRichMineralField,
                                   units.Neutral.PurifierRichMineralField750,
                                   units.Neutral.RichMineralField,
                                   units.Neutral.RichMineralField750
                               ]]
            scv = random.choice(idle_scvs)
            distances = self.get_distances(obs, mineral_patches, (scv.x, scv.y))
            mineral_patch = mineral_patches[np.argmin(distances)]
            return actions.RAW_FUNCTIONS.Harvest_Gather_unit(
                "now", scv.tag, mineral_patch.tag)
        return actions.RAW_FUNCTIONS.no_op()

    def build_supply_depot(self, obs):
        supply_depots = self.get_my_units_by_type(obs, units.Terran.SupplyDepot)
        scvs = self.get_my_units_by_type(obs, units.Terran.SCV)
        if (len(supply_depots) == 0 and obs.observation.player.minerals >= 100 and
                len(scvs) > 0):
            supply_depot_xy = (22, 26) if self.base_top_left else (35, 42)
            if(len(supply_depots)>1):
                supply_depot_xy = (24, 26) if self.base_top_left else (33, 42)
                #supply_depot_xy[1]+=4
            distances = self.get_distances(obs, scvs, supply_depot_xy)
            scv = scvs[np.argmin(distances)]
            return actions.RAW_FUNCTIONS.Build_SupplyDepot_pt(
                "now", scv.tag, supply_depot_xy)
        return actions.RAW_FUNCTIONS.no_op()

    def build_barracks(self, obs):
        completed_supply_depots = self.get_my_completed_units_by_type(
            obs, units.Terran.SupplyDepot)
        barrackses = self.get_my_units_by_type(obs, units.Terran.Barracks)
        scvs = self.get_my_units_by_type(obs, units.Terran.SCV)
        if (len(completed_supply_depots) > 0 and len(barrackses) <2 and
                obs.observation.player.minerals >= 150 and len(scvs) > 0):
            barracks_xy = (22, 21) if self.base_top_left else (35, 45)
            if(len(barrackses) > 0):
                barracks_xy = (25, 21) if self.base_top_left else (32, 45)
                #barracks_xy[1]+=4
            distances = self.get_distances(obs, scvs, barracks_xy)
            scv = scvs[np.argmin(distances)]
            return actions.RAW_FUNCTIONS.Build_Barracks_pt(
                "now", scv.tag, barracks_xy)
        return actions.RAW_FUNCTIONS.no_op()

    def train_marine(self, obs):
        completed_barrackses = self.get_my_completed_units_by_type(
            obs, units.Terran.Barracks)
        free_supply = (obs.observation.player.food_cap -
                       obs.observation.player.food_used)
        if (len(completed_barrackses) > 0 and obs.observation.player.minerals >= 100
                and free_supply > 0):
            barracks = self.get_my_units_by_type(obs, units.Terran.Barracks)[0]
            if barracks.order_length < 5:
                return actions.RAW_FUNCTIONS.Train_Marine_quick("now", barracks.tag)
        return actions.RAW_FUNCTIONS.no_op()

    def attack(self, obs):
        enemy_commandcenters = self.get_enemy_units_by_type(
            obs, units.Terran.CommandCenter)
        enemy_commandcenters.append(self.get_enemy_units_by_type(obs,units.Terran.OrbitalCommand))
        marines = self.get_my_units_by_type(obs, units.Terran.Marine)
        if len(marines) > 9:
                attack_xy = (38, 44) if self.base_top_left else (19, 23)
                distances = self.get_distances(obs, marines, attack_xy)
                marine = marines[np.argmax(distances)]
                x_offset = random.randint(-4, 4)
                y_offset = random.randint(-4, 4)
                if(len(enemy_commandcenters)>1):
                    attack_xy = (19, 44) if self.base_top_left else (38, 23)
                return actions.RAW_FUNCTIONS.Attack_pt(
                    "now", marine.tag, (attack_xy[0] + x_offset,attack_xy[1]+ y_offset))
        return actions.RAW_FUNCTIONS.no_op()

class TerranRandomAgent(TerranAgentWithRawActsAndRawObs):
    def step(self, obs):
        super(TerranRandomAgent, self).step(obs)
        action = random.choice(self.actions)
        return getattr(self, action)(obs)

class TerranRLAgentWithRawActsAndRawObs(TerranAgentWithRawActsAndRawObs):
    def __init__(self):
        super(TerranRLAgentWithRawActsAndRawObs, self).__init__()
        self.cumulative_scores = []  # 에피소드마다 누적 점수 기록
        self.win_rate = []  # 에피소드마다 승률 기록
        self.draw_rate=[]
        self.defeat_rate = []
        self.winrate=[]
        self.drawrate=[]
        self.defeatrate=[]
        self.gamereward=[]
        self.gamescore=[]
        self.bargame=[0,0,0]
        self.bargame[0]=0
        self.bargame[1]=0
        self.bargame[2]=0
        self.s_dim = 21
        self.a_dim = 6
        self.lr = 1e-4
        self.gamma = 1.0
        self.epsilon = 0.9
        self.x=25
        self.y=25
        self.qnetwork = NaiveMultiLayerPerceptron(input_dim=self.s_dim,
                           output_dim=self.a_dim,
                           num_neurons=[128],
                           hidden_act_func='ReLU',
                           out_act_func='Identity').to(device)

        self.data_file = 'rlagent_with_naive_dqn'
        if os.path.isfile(self.data_file + '.pt'):
            self.qnetwork.load_state_dict(torch.load(self.data_file + '.pt', map_location=device))

        self.dqn = NaiveDQN(state_dim=self.s_dim,
                             action_dim=self.a_dim,
                             qnet=self.qnetwork,
                             lr=self.lr,
                             gamma=self.gamma,
                             epsilon=self.epsilon).to(device)

        self.print_every = 50
        self.ema_factor = 0.5
        self.ema = EMAMeter(self.ema_factor)
        self.cum_reward = 0
        self.cum_loss = 0
        self.episode_count = 0
        self.writer=SummaryWriter()
        self.new_game()

    def reset(self):
        super(TerranRLAgentWithRawActsAndRawObs, self).reset()
        self.new_game()

    def new_game(self):
        self.base_top_left = None
        self.previous_state = None
        self.previous_action = None
        self.cum_reward = 0
        self.cum_loss = 0

    def get_state(self, obs):
        scvs = self.get_my_units_by_type(obs, units.Terran.SCV)
        idle_scvs = [scv for scv in scvs if scv.order_length == 0]
        command_centers = self.get_my_units_by_type(obs, units.Terran.CommandCenter)
        supply_depots = self.get_my_units_by_type(obs, units.Terran.SupplyDepot)
        completed_supply_depots = self.get_my_completed_units_by_type(
            obs, units.Terran.SupplyDepot)
        barrackses = self.get_my_units_by_type(obs, units.Terran.Barracks)
        completed_barrackses = self.get_my_completed_units_by_type(
            obs, units.Terran.Barracks)
        marines = self.get_my_units_by_type(obs, units.Terran.Marine)

        queued_marines = (completed_barrackses[0].order_length
        if len(completed_barrackses) > 0 else 0)

        free_supply = (obs.observation.player.food_cap -
                       obs.observation.player.food_used)
        can_afford_supply_depot = obs.observation.player.minerals >= 100
        can_afford_barracks = obs.observation.player.minerals >= 150
        can_afford_marine = obs.observation.player.minerals >= 100

        enemy_scvs = self.get_enemy_units_by_type(obs, units.Terran.SCV)
        enemy_idle_scvs = [scv for scv in enemy_scvs if scv.order_length == 0]
        enemy_command_centers = self.get_enemy_units_by_type(
            obs, units.Terran.CommandCenter)
        enemy_command_centers.append(self.get_enemy_units_by_type(obs,units.Terran.OrbitalCommand))
        #print(enemy_command_centers)
        enemy_supply_depots = self.get_enemy_units_by_type(
            obs, units.Terran.SupplyDepot)
        enemy_completed_supply_depots = self.get_enemy_completed_units_by_type(
            obs, units.Terran.SupplyDepot)
        enemy_barrackses = self.get_enemy_units_by_type(obs, units.Terran.Barracks)
        enemy_completed_barrackses = self.get_enemy_completed_units_by_type(
            obs, units.Terran.Barracks)
        enemy_marines = self.get_enemy_units_by_type(obs, units.Terran.Marine)

        return (len(command_centers),
                len(scvs),
                len(idle_scvs),
                len(supply_depots),
                len(completed_supply_depots),
                len(barrackses),
                len(completed_barrackses),
                len(marines),
                queued_marines,
                free_supply,
                can_afford_supply_depot,
                can_afford_barracks,
                can_afford_marine,
                len(enemy_command_centers),
                len(enemy_scvs),
                len(enemy_idle_scvs),
                len(enemy_supply_depots),
                len(enemy_completed_supply_depots),
                len(enemy_barrackses),
                len(enemy_completed_barrackses),
                len(enemy_marines))

    def step(self, obs):
        super(TerranRLAgentWithRawActsAndRawObs, self).step(obs)

        #time.sleep(0.5)

        state = self.get_state(obs)
        #print(state)
        state = torch.tensor(state).float().view(1, self.s_dim).to(device)
        action_idx = self.dqn.choose_action(state)
        action = self.actions[action_idx]
        done = True if obs.last() else False
        if self.previous_action is not None:
            loss = self.dqn.learn(self.previous_state.to(device),
                              torch.tensor(self.previous_action).view(1, 1).to(device),
                              torch.tensor(obs.reward).view(1, 1).to(device),
                              state.to(device),
                              torch.tensor(done).float().view(1, 1).to(device)
                              )
            self.cum_loss += loss.detach().numpy()
        self.cum_reward += obs.reward
        self.previous_state = state
        self.previous_action = action_idx

        if obs.last():
            self.episode_count = self.episode_count + 1
            torch.save(self.dqn.qnet.state_dict(), self.data_file + '.pt')
            self.cumulative_scores.append(obs.observation['score_cumulative'][0])
            win = 1 if self.cum_reward == 1 else 0
            draw = 1 if self.cum_reward == 0 else 0
            defeat = 1 if self.cum_reward == -1 else 0
            self.win_rate.append(win)
            self.draw_rate.append(draw)
            self.defeat_rate.append(defeat)
            total_episodes = len(self.win_rate)
            win_count = sum(self.win_rate)
            self.winrate.append((win_count / total_episodes))
            draw_count = sum(self.draw_rate)
            defeat_count = sum(self.defeat_rate)
            self.drawrate.append((draw_count / total_episodes))
            self.defeatrate.append((defeat_count / total_episodes))
            self.ema.update(self.cum_reward)
            writer.add_scalar("Loss/online", self.cum_loss/obs.observation.game_loop, self.episode_count)
            writer.add_scalar("Score", self.ema.s, self.episode_count)
            self.gamereward.append(self.cum_reward)
            self.gamescore.append(obs.observation['score_cumulative'][0])
            if(self.cum_reward==1):
                self.bargame[0]=self.bargame[0]+1
            if(self.cum_reward==0):
                self.bargame[1]=self.bargame[1]+1
            if(self.cum_reward==-1):
                self.bargame[2]=self.bargame[2]+1
            if self.episode_count % self.print_every == 0:
                print("Episode {} || EMA: {} || EPS : {}".format(self.episode_count, self.ema.s, self.dqn.epsilon))

            if self.episode_count >= 150:
                self.dqn.epsilon *= 0.999

        return getattr(self, action)(obs)
    def plot_results(self):
    # 에피소드 수에 따른 누적 점수 선 그래프
        plt.figure(figsize=(10, 5))
        plt.plot(range(1, len(self.cumulative_scores) + 1), self.cumulative_scores, label='Cumulative Score', linestyle='-', marker='o')
        plt.title('Cumulative Score over Episodes')
        plt.xlabel('Episode')
        plt.ylabel('Cumulative Score')
        plt.legend()
        plt.grid(True)
        plt.show()

    def plot_win_rate(self):
    # 에피소드 수에 따른 승률 그래프
        total_episodes = len(self.winrate)
        plt.figure(figsize=(10, 5))
        plt.plot(range(1, total_episodes + 1), self.winrate, label='Win Rate', linestyle='-', marker='o', color='g')
        plt.plot(range(1, total_episodes + 1), self.drawrate, label='Draw Rate', linestyle='-', marker='o', color='b')
        plt.plot(range(1, total_episodes + 1), self.defeatrate, label='Defeat Rate', linestyle='-', marker='o', color='r')
        plt.title('Win/Draw/Defeat Rates over Episodes')
        plt.xlabel('Episode')
        plt.ylabel('Rate')
        plt.ylim([0, 1])
        plt.legend()
        plt.grid(True)
        plt.show()

def main(unused_argv):
    agent1 = TerranRLAgentWithRawActsAndRawObs()
    try:
       with sc2_env.SC2Env(
               map_name="Simple64",
               players=[sc2_env.Agent(sc2_env.Race.terran),
                        sc2_env.Bot(sc2_env.Race.terran,
                                    sc2_env.Difficulty.very_easy)],
               agent_interface_format=features.AgentInterfaceFormat(
                   action_space=actions.ActionSpace.RAW,
                   use_raw_units=True,
                   raw_resolution=64,
               ),
               step_mul=8,
               disable_fog=True,
               visualize=False
       ) as env:
           run_loop.run_loop([agent1], env, max_episodes=1000)
    except KeyboardInterrupt:
       pass
    agent1.plot_results()
    agent1.plot_win_rate()
    """plt.plot(agent1.gamescore)
    plt.xlabel('Episode')
    plt.ylabel('Cumulative score')
    plt.title('Agent Performance')
    plt.show()       
    plt.bar(len(agent1.bargame),agent1.bargame)
    plt.xlabel('Result')
    plt.ylabel('reward')
    plt.title('Agent Performance')
    plt.show()"""
if __name__ == "__main__":
    app.run(main)