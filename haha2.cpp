#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
using namespace std;
const int maxChannelSize = 5; // the max number of channel does a road have
const int maxRoadSize = 210;
const int maxCarSize = 1e5+10;
const int maxCrossSize = 100;
const int maxWaitingCar = 10010;
const int VGeler = 1;// if debug VGeler = 1; else VGeler = 0;

/*  direction
 *  turn left --> 0;  go straight --> 1;  turn right --> 2;
 *              | turn |
 *              | left |
 *              |   =  |
 *              |   0  |
 *   -------                -------------------
 *   my road                  go straight = 1
 *   -------                -------------------
 *              | turn |
 *              | right|
 *              |   =  |
 *              |   2  |
 */
vector <int> roadPosToId;
vector <int> carPosToId;
vector <int> crossPosToId;

//read and write file start

char stringBuff[1024];

/*
struct CAR
{
    int cid; //car id;
    int from;
    int to;
    int speed;
    int planTime; //the time when leave source

    int arrayId; //output message of paths array id
    int position; //position in the road
    char direction; //direction in the next cross, car go termination if direction = #

    CAR() {}
    CAR(int _cid, int _from, int _to, int _speed, int _planTime, int _arrayId) :
        cid(_cid), from(_from), to(_to), speed(_speed), planTime(_planTime), arrayId(_arrayId) {}
};
*/
//_______________________________________________by VGeler_begin_struct CAR

struct LOCATION{
    int roadPos;  //road[roadPos] = roadId;
    int channel;  //Representing the priority
    int distance; //how many distance between the car and the end of the road, distance = 0 means the car ready to drive into a new road
    LOCATION() {}
    LOCATION(int _rp, int _channel, int _dis) :
        roadPos(_rp), channel(_channel), distance(_dis) {}

    void init(){
        roadPos = -1;
        channel = -1;
        distance = -1;
    }

    void out(){
        cout << "the car locate at Road " << roadPos << " , Channel " << channel <<". And the distance to the end is : " << distance << endl;
//        cout << "the car locate at Road " << roadPosToId[roadPos] << " , Channel " << channel <<". And the distance to the end is : " << distance << endl;
    }
};
struct CAR
{
    int cid; //car id;
    int from;
    int to;
    int maxSpeed;
    int nowSpeed;
    int planTime; //the time plan to set off
    int setoffTime; //the time actually set off     setoffTime = -1 --> wait to leave

    int arrayId; //output message of paths array id
    int position; //position in the road
    LOCATION location;
    char direction; //direction in the next cross, car go termination if direction = #

    CAR() {}
    CAR(int _cid, int _from, int _to, int _speed, int _planTime, int _arrayId) :
        cid(_cid), from(_from), to(_to), maxSpeed(_speed), planTime(_planTime), arrayId(_arrayId) {}

    void init(int _id, int _from, int _to, int _speed, int _plantime){
        cid = _id;
        from = _from;
        to = _to;
        maxSpeed = _speed;
        nowSpeed = 0;
        planTime = _plantime;
        setoffTime = -1;
        arrayId = -1;
        position = -1;
        location = LOCATION(maxRoadSize, 0, 0);
        direction = '*';
    }

    void out(){
        cout << "Car : {" << endl;
        cout << "\tCar Id = " << cid << endl;
        cout << "\tfrom cross " << from << " to cross " << to <<endl;
        cout << "\tmaxSpeed = " << maxSpeed << endl;
        cout << "\tnowSpeed = " << nowSpeed << endl;
        cout << "\tplanTime = " << planTime << endl;
        cout << "\tsetoffTime = " << setoffTime << endl;
        cout << "\t";   location.out();
        cout << "\tposcition = " << position << endl;


        cout << "}" << endl << endl;
    }
};
vector < CAR > car;
map < int, int > carIdToPos; // map<CarID, pos> --> car[pos].cid = CarID

//_______________________________________________by VGeler_end_struct CAR

/*
struct ROAD
{
    int Rid;
    int length;
    int speed;
    int channel;
    int from;
    int to;
    int isDuplex;

    ROAD() {}
    ROAD(int _Rid, int _length, int _speed, int _channel, int _from, int _to, int _isDuplex) :
        Rid(_Rid), length(_length), speed(_speed), channel(_channel), from(_from), to(_to), isDuplex(_isDuplex) {}
};
*/
//_______________________________________________by VGeler_begin_struct ROAD
struct ROAD
{
    int Rid;//road_id
    int length;
    int speed;
    int channel;//how many channel
    vector <int> cn[maxChannelSize];//car ID in each channel
    int from;
    int to;
    int isDuplex;
    int direction[3]; // pos!! not road id

    ROAD() {}
    ROAD(int _Rid, int _length, int _speed, int _channel, int _from, int _to, int _isDuplex) :
        Rid(_Rid), length(_length), speed(_speed), channel(_channel), from(_from), to(_to), isDuplex(_isDuplex) {}

    void init(int _id, int _len, int _maxSpeed, int _channel, int _startID, int _endID, int _bid){
        Rid = _id;
        length = _len;
        speed = _maxSpeed;
        channel = _channel;
        for ( int i = 0 ; i < _channel ; i++ ) {
            cn[i].clear();
            cn[i].reserve(_len);
        }
        from = _startID;
        to = _endID;
        isDuplex = _bid;

        for (int i = 0 ; i < 3; i++ ) direction[i] = -1;
    }

    void out(){
        cout << "Road : {" << endl;
        cout << "\tRoadID = " << Rid << endl;
        if (isDuplex) cout << "\tTwo-way road" << endl;
        cout << "\tFrom cross " << from << " to cross " << to << endl;
        cout << "\tLimit max speed = " << speed << endl;
        cout << "\tRoad Length = " << length << endl;
        for (int i = 0 ; i < channel ; i++ ){
            cout<< "\tchannel " << i+1 << " : " << cn[i].size() << endl;
        }
        if (direction[0] >= 0) cout << "\tturn left is Road " << roadPosToId[direction[0]] <<endl;
        if (direction[1] >= 0) cout << "\tgo straight is Road " << roadPosToId[direction[1]] <<endl;
        if (direction[2] >= 0) cout << "\tturn right is Road " << roadPosToId[direction[2]] <<endl;
        cout << "}" << endl << endl;
    }
};//bidway road split into two roads
vector <ROAD> road;
map <int, int> roadIdToPos; // map<roadID, pos> --> road[pos].Rid = roadID
//_______________________________________________by VGeler_end_struct ROAD

/*
struct Cross
{
    int Cid;
    int Rid[8];

    Cross() {}
    Cross(int _Cid, int _Rid0, int _Rid1, int _Rid2, int _Rid3)
        {Cid = _Cid; Rid[0] = _Rid0; Rid[1] = _Rid1; Rid[2] = _Rid2; Rid[3] = _Rid3;}
};
*/
//_______________________________________________by VGeler_begin_struct CROSS
struct CROSS
{
    int Cid;
    int Rid[8]; // road id !! not pos
    vector <int> waitingCar; // Clockwise

    CROSS() {}
    CROSS(int _Cid, int _Rid0, int _Rid1, int _Rid2, int _Rid3)
        {Cid = _Cid; Rid[0] = _Rid0; Rid[1] = _Rid1; Rid[2] = _Rid2; Rid[3] = _Rid3;}

    void init(int _Cid, int _Rid0, int _Rid1, int _Rid2, int _Rid3){
        Cid = _Cid;
        Rid[0] = _Rid0;     Rid[4] = _Rid0;
        Rid[1] = _Rid1;     Rid[5] = _Rid1;
        Rid[2] = _Rid2;     Rid[6] = _Rid2;
        Rid[3] = _Rid3;     Rid[7] = _Rid3;
        waitingCar.clear();
        waitingCar.reserve(maxWaitingCar);
    }
};
vector <CROSS> cross;
map <int, int> crossIdToPos; // map<crossID, pos> --> cross[pos].Rid = CID
//_______________________________________________by VGeler_end_struct CROSS

int getRoadPos(int rid, int to){
    if (rid == -1) return -1;
    int roadPos = roadIdToPos[rid];
    if (road[roadPos].to != to) roadPos++;
    return roadPos;
}
void ConnectRoadByCross(int rid, int cid, CROSS cr){
    int roadPos = getRoadPos(rid, cid);
    int pp = -1;
    for (int i = 0 ; i < 4; i++) if (cr.Rid[i] == rid) {pp = i; break;}
    road[roadPos].direction[0] = getRoadPos(cr.Rid[pp+1], cid);
    road[roadPos].direction[1] = getRoadPos(cr.Rid[pp+2], cid);
    road[roadPos].direction[2] = getRoadPos(cr.Rid[pp+3], cid);
}






struct Graph
{
    vector<ROAD> roads;
    vector<CROSS> crosses;

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
            roads.push_back(ROAD(id, length, speed, channel, from, to, isDuplex));
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
            crosses.push_back(CROSS(id, roadId0, roadId1, roadId2, roadId3));
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

struct Edge
{
    int eid; //edge id
    int length;
    int speed;
    int channel;
    vector< vector<CAR> > road;

    double penalize;

    Edge() {}
//    Edge(int _eid, )
};

struct Point
{
    int pid; //point id

    vector<Edge> edge; //0-left, 1-right, 2-up, 3-down, eid = -1 -> no edge
};

//----------------------------------------------------------------------------------------VGeler_Read_begin
int initialRoadNumber;
void ReadRoad(){
    freopen("road.txt", "r", stdin);

    road.clear();
    road.reserve(maxRoadSize);
    roadPosToId.clear();
    roadPosToId.reserve(maxRoadSize);
    roadIdToPos.clear();

    scanf("%d", &initialRoadNumber);
    roadIdToPos.clear();

    for (int i = 0 ; i < initialRoadNumber ; i++ ){
        int id, len, maxSpeed, channel, startID, endID, bid;
        scanf("%d%d%d%d%d%d%d",&id, &len, &maxSpeed, &channel, &startID, &endID, &bid);

        roadIdToPos[id] = road.size();
        roadPosToId.push_back(id);

        ROAD r;     r.init(id, len, maxSpeed, channel, startID, endID, bid);
        road.push_back(r);
        if (bid){
            roadPosToId.push_back(id);
            r.init(id, len, maxSpeed, channel, endID, startID, bid);
            road.push_back(r);
        }
    }
//    if (VGeler){
//        for (auto &r : road) r.out();
//        for (auto &it : roadPosToId) cout << it << ",";
//        cout<<endl;
//    }
    fclose(stdin);
}

int initialCarNumber;
void ReadCar(){
    freopen("car.txt","r",stdin);

    car.clear();
    car.reserve(maxCarSize);
    carPosToId.clear();
    carPosToId.reserve(maxCarSize);
    carIdToPos.clear();

    scanf("%d", &initialCarNumber);

    for (int i = 0 ; i < initialCarNumber ; i++ ){
        int id, from, to, speed, plantime;
        scanf("%d%d%d%d%d", &id, &from, &to, &speed, &plantime);

        carIdToPos[id] = car.size();
        carPosToId.push_back(id);

        CAR cc;     cc.init(id, from, to, speed, plantime);
        car.push_back(cc);

    }
//    if (VGeler){
//        for (auto &cc : car) cc.out()
//        for (auto &it : carPosToId) cout << it << ",";
//        cout<<endl;
//    }
    fclose(stdin);
}

int initialCrossNumber;
void ReadCross(){
    freopen("cross.txt","r",stdin);

    cross.clear();
    cross.reserve(maxCrossSize);
    crossPosToId.clear();
    crossPosToId.reserve(maxCrossSize);
    crossIdToPos.clear();

    scanf("%d", &initialCrossNumber);

    for (int i = 0 ; i < initialCrossNumber ; i++ ){
        int id, rid[4];
        scanf("%d%d%d%d%d",&id, &rid[0], &rid[1], &rid[2], &rid[3]);

        crossIdToPos[id] = cross.size();
        crossPosToId.push_back(id);

        CROSS cr;       cr.init(id, rid[0], rid[1], rid[2], rid[3]);
        for (int j = 0 ; j < 4 ; j++) if (rid[j] >= 0) ConnectRoadByCross(rid[j], id, cr);
        cross.push_back(cr);
    }
    if (VGeler){
        for (auto &r : road) r.out();
        for (auto &it : roadPosToId) cout << it << ",";
        cout<<endl;
    }
    fclose(stdin);
}
//------------------------------------------------------------------------------------------------VGeler_read_end
int main(){
    freopen("out.txt","w",stdout);
    ReadRoad();
    ReadCar();
    ReadCross();
    fclose(stdout);
    return 0;
}
//VGeler_end
/*
int main()
{
    Graph G;
    G.BuildGraph("1-map-training-1\\road.txt", "1-map-training-1\\cross.txt");
//    G.PrintGraph();
    ReadTasks("1-map-training-1\\car.txt");
//    PrintTask();
    return 0;
}
*/
