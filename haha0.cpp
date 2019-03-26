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

    void ReadRoads(char *fileDir)
    {
        FILE *file = fopen(fileDir, "r");

        if(file == NULL)
        {
            cout <<"open " <<fileDir <<" failed!" <<endl;
            return ;
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
    }

    void ReadCrosses(char *fileDir)
    {
        FILE *file = fopen(fileDir, "r");

        if(file == NULL)
        {
            cout <<"open " <<fileDir <<" failed!" <<endl;
            return ;
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
    }

    void BuildGraph(char *roadDir, char *crossDir)
    {
        ReadRoads(roadDir);
        ReadCrosses(crossDir);
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

vector<Task> tasks;

void ReadTasks(char *fileDir)
{
    FILE *file = fopen(fileDir, "r");

    if(file == NULL)
    {
        cout <<"open " <<fileDir <<" failed!" <<endl;
        return ;
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
}

void PrintTask()
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

vector< vector<int> > carPath;

struct Car
{
    int cid; //car id;
    int from;
    int to;
    int speed;
    int planTime; //the time when leave source

    int arrayId; //output message of paths array id
    int position; //position in the road
    char direction; //direction in the next cross, car go termination if direction = #

    Car() {}
    Car(int _cid, int _from, int _to, int _speed, int _planTime, int _arrayId) :
        cid(_cid), from(_from), to(_to), speed(_speed), planTime(_planTime), arrayId(_arrayId) {}
};

struct Edge
{
    int eid; //edge id
    int length;
    int speed;
    int channel;
    vector< vector<Car> > road;

    double penalize;

    Edge() {}
    Edge(int _eid, )
};

struct Point
{
    int pid; //point id

    vector<Edge> edge; //0-left, 1-right, 2-up, 3-down, eid = -1 -> no edge
};

struct Edge
{
    int eid;
};


int main()
{
    Graph G;
    G.BuildGraph("1-map-training-1\\road.txt", "1-map-training-1\\cross.txt");
//    G.PrintGraph();
    ReadTasks("1-map-training-1\\car.txt");
//    PrintTask();
    return 0;
}
