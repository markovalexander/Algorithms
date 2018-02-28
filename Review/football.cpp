//
//  main.cpp
//  FootballPlayers
//
//  Created by Александр Марков on 03/10/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

struct Player;
class TeamSegment;

bool ComparePlayersBySkill(const Player&, const Player&);
bool ComparePlayersByPosition(const Player&, const Player&);
inline bool CanAdd(const Player& last, const Player& first, const Player& second);
int64_t FindTeamEffectiveness(const std::vector<Player>& team);

namespace sort {
    inline size_t GetLeftChild(size_t);
    inline size_t GetRightChild(size_t);

    template<typename Iterator, typename Compare = std::less<>>
    void Heapify(Iterator first, Iterator last, size_t current, Compare comp = Compare());
    
    template<typename Iterator, typename Compare = std::less<>>
    void MakeHeap(Iterator first, Iterator last, Compare comp = Compare());
    
    template<typename Iterator, typename Compare = std::less<>>
    void MySort(Iterator first, Iterator last, Compare comp = Compare());
} // namespace sort


std::vector<Player> ReadInput(std::istream& in_stream = std::cin);
void PrintAnswer(const std::vector<Player>& effective_team,
                 std::ostream& out_stream = std::cout);

std::vector<Player> BuildMostEffectiveSolidaryTeam(std::vector<Player> players);

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    std::vector<Player> players = ReadInput();
    std::vector<Player> effective_team = BuildMostEffectiveSolidaryTeam(players);
    PrintAnswer(effective_team);
    return 0;
}

struct Player {
    int64_t skill;
    size_t position;
};

class TeamSegment {
public:
    TeamSegment(size_t l_p, size_t r_p, int64_t skill)
        : left_player_(l_p), right_player_(r_p),
        skill_(skill)
    {}
    
    void PopLeftPlayer(int64_t skill) {
        ++left_player_;
        skill_ -= skill;
    }
    
    void PushRightPlayer(int64_t skill) {
        ++right_player_;
        skill_ += skill;
    }
    
    const size_t GetLeftPlayer() const {
        return left_player_;
    }
    
    const size_t GetRightPlayer() const {
        return right_player_;
    }
    
    const int64_t GetSkill() const {
        return skill_;
    }
    
private:
    size_t left_player_;
    size_t right_player_;
    int64_t skill_;
};

bool ComparePlayersBySkill(const Player& first, const Player& second) {
    return first.skill < second.skill;
}

bool ComparePlayersByPosition(const Player& first, const Player& second) {
    return first.position < second.position;
}

bool CanAdd(const Player& last, const Player& first, const Player& second) {
    return last.skill <= first.skill + second.skill;
}

int64_t FindTeamEffectiveness(const std::vector<Player>& team) {
    int64_t effectiveness{0};
    for (const Player& player: team) {
        effectiveness += player.skill;
    }
    return effectiveness;
}

size_t sort::GetLeftChild(size_t pos) {
    return 2 * pos + 1;
}

size_t sort::GetRightChild(size_t pos) {
    return 2 * pos + 2;
}

template<typename Iterator, typename Compare>
void sort::Heapify(Iterator first, Iterator last, size_t current, Compare comp) {
    size_t size = std::distance(first, last);
    size_t largest = current;
    
    if (GetLeftChild(current) < size
        && comp(*(first + largest), *(first + GetLeftChild(current))))
    {
        largest = GetLeftChild(current);
    }
    if (GetRightChild(current) < size
        && comp(*(first + largest), *(first + GetRightChild(current))))
    {
        largest = GetRightChild(current);
    }
    
    if (largest == current) {
        return;
    }
    std::swap(*(first + current), *(first + largest));
    Heapify(first, last, largest, comp);
}

template<typename Iterator, typename Compare>
void sort::MakeHeap(Iterator first, Iterator last, Compare comp) {
    int size = static_cast<int>(std::distance(first, last));
    for (int i = (size + 1) / 2; i >= 0; --i) {
        Heapify(first, last, static_cast<size_t>(i), comp);
    }
}

template<typename Iterator, typename Compare>
void sort::MySort(Iterator first, Iterator last, Compare comp) {
    MakeHeap(first, last, comp);
    int size = static_cast<int>(std::distance(first, last));
    for (int current_pos = size - 1; current_pos >= 0; --current_pos) {
        std::swap(*(first), *(std::next(first, current_pos)));
        Heapify(first, first + current_pos, 0, comp);
    }
}

std::vector<Player> ReadInput(std::istream& instream) {
    size_t num_players;
    instream >> num_players;
    
    std::vector<Player> players(num_players);
    for (size_t i = 0; i < num_players; ++i) {
        int64_t skill;
        instream >> skill;
        players[i] = {skill, i + 1};
    }
    return players;
}

std::vector<Player> BuildMostEffectiveSolidaryTeam(std::vector<Player> players) {
    if (players.size() <= 2) {
        return players;
    }

    sort::MySort(players.begin(), players.end(), ComparePlayersBySkill);
    
    TeamSegment current_team(0, 2, players[0].skill + players[1].skill);
    TeamSegment best_team(0, 2, players[0].skill + players[1].skill);

    while (current_team.GetRightPlayer() < players.size()) {
        if (CanAdd(players[current_team.GetRightPlayer()],
                   players[current_team.GetLeftPlayer()],
                   players[current_team.GetLeftPlayer() + 1]))
        {
            current_team.PushRightPlayer(players[current_team.GetRightPlayer()].skill);
            if (current_team.GetSkill() >= best_team.GetSkill()) {
                best_team = current_team;
            }
        } else {
            current_team.PopLeftPlayer(players[current_team.GetLeftPlayer()].skill);
        }
    }
    std::vector<Player> answer(players.begin() + best_team.GetLeftPlayer(),
                               players.begin() + best_team.GetRightPlayer());
    
    sort::MySort(answer.begin(), answer.end(), ComparePlayersByPosition);
    return answer;
}

void PrintAnswer(const std::vector<Player>& answer, std::ostream& out_stream) {
    int64_t effectiveness = FindTeamEffectiveness(answer);
    out_stream << effectiveness << "\n";
    for (const Player& player : answer) {
        out_stream << player.position << " ";
    }
}
