#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;


//read and write file start

char stringBuff[1024];

struct Road
{
    int Rid;
    int length;
    int speed;
    int channel;
    int from;
    int to;
    int isDuplex;

    Road() {}
    Road(int _Rid, int _length, int _speed, int _channel, int _from, int _to, int _isDuplex) :
        Rid(_Rid), length(_length), speed(_speed), channel(_channel), from(_from), to(_to), isDuplex(_isDuplex) {}
};
struct Cross
{
    int Cid;
    int Rid[4];

    Cross() {}
    Cross(int _Cid, int _Rid0, int _Rid1, int _Rid2, int _Rid3)
        {Cid = _Cid; Rid[0] = _Rid0; Rid[1] = _Rid1; Rid[2] = _Rid2; Rid[3] = _Rid3;}
};



struct Graph
{
    vector<Road> roads;
    vector<Cross> crosses;

    vector<Road> ReadRoads(char *fileDir)
    {
        vector<Road> roads;
        FILE *file = fopen(fileDir, "r");

        if(file == NULL)
        {
            cout <<"open " <<fileDir <<" failed!" <<endl;
            return roads;
        }

        while(fgets(stringBuff, 1024, file), stringBuff[0] == '#') ;

        do
        {
            int id, length, speed, channel, from, to, isDuplex;
            sscanf(stringBuff, "%*c%d,%d,%d,%d,%d,%d,%d", &id, &length, &speed, &channel, &from, &to, &isDuplex);
            roads.push_back(Road(id, length, speed, channel, from, to, isDuplex));
//            cout <<stringBuff <<endl;
//            cout <<id <<" " <<length <<" " <<speed <<" " <<channel <<" " <<from <<" " <<to <<" " <<isDuplex <<endl;
        }while(fgets(stringBuff, 1024, file));

        cout <<"read file " <<fileDir <<" successful, total lines : " <<roads.size() <<endl;

        fclose(file);
        return roads;
    }

    vector<Cross> ReadCrosses(char *fileDir)
    {
        vector<Cross> crosses;
        FILE *file = fopen(fileDir, "r");

        if(file == NULL)
        {
            cout <<"open " <<fileDir <<" failed!" <<endl;
            return crosses;
        }

        while(fgets(stringBuff, 1024, file), stringBuff[0] == '#') ;

        do
        {
            int id, roadId0, roadId1, roadId2, roadId3;
            sscanf(stringBuff, "%*c%d,%d,%d,%d,%d", &id, &roadId0, &roadId1, &roadId2, &roadId3);
            crosses.push_back(Cross(id, roadId0, roadId1, roadId2, roadId3));
//            cout <<stringBuff <<endl;
//            cout <<id <<" " <<roadId0 <<" " <<roadId1 <<" " <<roadId2 <<" " <<roadId3 <<endl;
        }while(fgets(stringBuff, 1024, file));

        cout <<"read file " <<fileDir <<" successful, total lines : " <<crosses.size() <<endl;

        fclose(file);
        return crosses;
    }

    void BuildGraph(char *roadDir, char *crossDir)
    {
        roads = ReadRoads(roadDir);
        crosses = ReadCrosses(crossDir);
    }

    void PrintGraph()
    {
        FILE *edgelist, *pointlist;

        edgelist = fopen("edgelist.csv", "w");
        pointlist = fopen("pointlist.csv", "w");

        fprintf(pointlist, "id,label\n");
        for(const auto &cross : crosses)
        {
            fprintf(pointlist, "%d,%d\n", cross.Cid, cross.Cid);
        }

        fprintf(edgelist, "source,target,weight,type\n");
        for(const auto &road : roads)
        {
            fprintf(edgelist, "%d,%d,%d,%s\n", road.from, road.to, road.channel, road.isDuplex ? "undirected" : "directed");
        }

        fclose(edgelist);
        fclose(pointlist);
    }
};

struct Task
{
    int Tid;
    int from;
    int to;
    int speed;
    int planTime;

    Task() {}
    Task(int _id, int _from, int _to, int _speed, int _planTime) :
        Tid(_id), from(_from), to(_to), speed(_speed), planTime(_planTime) {}
};

vector<Task> ReadTasks(char *fileDir)
{
    vector<Task> tasks;
    FILE *file = fopen(fileDir, "r");

    if(file == NULL)
    {
        cout <<"open " <<fileDir <<" failed!" <<endl;
        return tasks;
    }

    while(fgets(stringBuff, 1024, file), stringBuff[0] == '#') ;

    do
    {
        int id, from, to, speed, planTime;
        sscanf(stringBuff, "%*c%d,%d,%d,%d,%d", &id, &from, &to, &speed, &planTime);
        tasks.push_back(Task(id, from, to, speed, planTime));

//        if(rand() < RAND_MAX / 100)
//        {
//            cout <<stringBuff <<endl;
//            cout <<id <<" " <<from <<" " <<to <<" " <<speed <<" " <<planTime <<endl;
//        }
    }while(fgets(stringBuff, 1024, file));

    cout <<"read file " <<fileDir <<" successful, total lines : " <<tasks.size() <<endl;

    fclose(file);
    return tasks;
}

void PrintTask(const vector<Task> &tasks)
{
    FILE *file = fopen("tasklist.csv", "w");

    fprintf(file, "source,target,type,label\n");

    for(const auto &task : tasks)
    {
        fprintf(file, "%d,%d,directed,%d\n", task.from, task.to, task.planTime);
    }

    fclose(file);
}

//read and write file end


//construct definition start

struct CAR
{
    int cid; //car id;
    int from;
    int to;
    int speed;
    int planTime; //the time when leave source

    int arrayId; //output message of paths array id
    int startTime;
    int prePoint;
    int position; //position in the road
    char direction; //direction in the next cross, car go termination if direction = #

    CAR() {}
    CAR(int _cid, int _from, int _to, int _speed, int _planTime, int _arrayId) :
        cid(_cid), from(_from), to(_to), speed(_speed), planTime(_planTime), arrayId(_arrayId) {}
};

struct EDGE
{
    int eid; //edge id
    int length;
    int speed;
    int channel;
    int from;
    int to;

    vector< vector<CAR> > road;

    EDGE() {}
    EDGE(int _eid, int _from, int _to, int _length, int _speed, int _channel) :
        eid(_eid), from(_from), to(_to), length(_length), speed(_speed), channel(_channel)
        {
            road = vector< vector<CAR> > (_channel);
        }
};

struct POINT
{
    int pid; //point id
    vector<int> edge;

    vector<CAR> carPark;
    vector<CAR> carPort;

    POINT() {}
    POINT(int _pid) :
        pid(_pid)
        {
            edge = vector<int> (8, -1);
        }
};

//construction definition end


// function

void AllInitialize(const vector<Road> &roads, const vector<Cross> &crosses, const vector<Task> &tasks,
                   vector<EDGE> &edges, vector<POINT> &points)
{
    carPath = vector< vector<int> > (tasks.size());
    edges.reserve(roads.size() * 2);
    points.reserve(crosses.size());

    map<int, int> edge2Array;

    for(int i = 0; i < roads.size(); i++)
    {
        const auto &road = roads[i];
        edges.push_back(EDGE(road.Rid, road.from, road.to, road.length, road.speed, road.channel));
        edge2Array[road.Rid] = edges.size() - 1;

        if(road.isDuplex)
        {
            edges.push_back(EDGE(road.Rid, road.to, road.from, road.length, road.speed, road.channel));
        }
    }

    for(int i = 0; i < crosses.size(); i++)
    {
        const auto &cross = crosses[i];
        points.push_back(POINT(cross.Cid));

        POINT &point = points[points.size() - 1];

        for(int j = 0; j < 4; j++)
        {
            if(cross.Rid[j] == -1)
            {
                point.edge[j << 1] = -1;
                point.edge[j << 1 | 1] = -1;
            }
            else
            {
                int mapId = edge2Array[cross.Rid[j]];

                point.edge[j << 1] = mapId;

                if(edges[mapId].isDuplex)
                {
                    point.edge[j << 1 | 1] = mapId + 1;
                }
                else
                {
                    point.edge[j << 1 | 1] = -1;
                }

                if(edges[mapId].from != point.pid)
                {
                    swap(point.edge[j << 1], point.edge[j << 1 | 1]);
                }
            }
        }
    }


    for(int i = 0; i < tasks.size(); i++)
    {
        const Task &task = tasks[i];
        int idx = task.from;
        points[idx].carPark.push_back(CAR(task.Tid, task.from, task.to, task.speed, task.planTime, i));
    }
}


void Planning(vector<POINT> &points, vector<EDGE> &edges, vector< vector<int> > &carPath) //<--  main function
{
    map<int, int> maps;


}

int Scheduling(vector<POINT> points, vector<EDGE> edges, vector< vector<int> > &carPath)
{
    vector<Car> runningCar;

}

int RunTest(vector< vector<int> > &carPath)
{
    Graph G;
    vector<Task> tasks;

    G.BuildGraph("1-map-training-1\\road.txt", "1-map-training-1\\cross.txt");
//    G.PrintGraph();
    tasks = ReadTasks("1-map-training-1\\car.txt");
//    PrintTask(tasks);
    vector<EDGE> edges;
    vector<POINT> points;
    vector< vector<int> > carPath;

    AllInitialize(G.roads, G.crosses, tasks, edges, points);

    int runTime = Scheduling(points, edges, carPath);

    return runTime;
}

int main()
{
    int ans;
    vector< vector<int> > carPath;
    ans = RunTest(carPath);
    return 0;
}
