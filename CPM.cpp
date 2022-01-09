/* Finished in the semester of 2020 spring, before final exam of Economics and Management. */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <cassert>

using namespace std;

constexpr int MAXN(105);
constexpr int INF(0x7fffffff);
struct Edge
{
    int to, w;
};

struct Vertex
{
    char ID;
    int DU;                         // duration of event
    int ES, EF, LS = INF, LF = INF; // Earlist Start/Finish, Latest Start/Finish
    int TF;                         // Total Float, TF = LS - ES = LF - EF
    int FF = INF;                   // Free Float
    vector<Edge> to, frm;           // to:邻接表, frm: 逆邻接表
    int ESpre = -1;                 // CriticalPath
} event[MAXN];

int nEvent, nEdge;

int indeg[MAXN], outdeg[MAXN];
vector<int> topo; // Topological Sort

bool TopologicalSort()
{
    queue<int> q;
    for (int i = 0; i < nEvent; i++)
    {
        if (indeg[i] == 0)
            q.push(i);
    }
    int cnt = 0;
    while (!q.empty())
    {
        int cur = q.front();
        q.pop();
        cnt++;
        topo.push_back(cur);
        for (Edge edge : event[cur].to)
        {
            indeg[edge.to]--;
            if (indeg[edge.to] == 0)
            {
                q.push(edge.to);
            }
        }
    }
    return cnt == nEvent;
}

void dispAll()
{
    for (int i = 0; i < nEvent; i++)
    {
        Vertex &ev = event[i];
        printf("%5d %5d %5d\n", ev.ES, ev.TF, ev.EF);
        printf("%5c %5c %5d\n", 32, ev.ID, ev.DU);
        printf("%5d %5d %5d\n", ev.LS, ev.FF, ev.LF);
        printf("\n");
    }
}

vector<int> criticalpath;
void getCriticalPath(int ed)
{
    stack<int> rpath;
    for (int cur = ed; cur != -1; cur = event[cur].ESpre)
    {
        rpath.push(cur);
    }
    while (!rpath.empty())
    {
        criticalpath.push_back(rpath.top());
        rpath.pop();
    }
}

int main()
{
    scanf("%d %d", &nEvent, &nEdge);
    for (int i = 0; i < nEvent; i++)
    {
        char c;
        int x;
        scanf(" %c %d", &c, &x);
        event[c - 'A'].ID = c;
        event[c - 'A'].DU = x;
    }
    for (int i = 0; i < nEdge; i++)
    {
        char c1, c2;
        char rel[5];
        int det = 0;
        scanf(" %c %c %s", &c1, &c2, rel);
        int i1 = c1 - 'A', i2 = c2 - 'A';
        if (rel[0] == '0')
            det = 0;
        else
        {
            scanf("%d", &det);
            // ensure `FS`
            if (rel[0] == 'S')
                det -= event[i1].DU;
            if (rel[1] == 'F')
                det -= event[i2].DU;
        }
        event[i1].to.push_back({i2, det});
        event[i2].frm.push_back({i1, det});
        indeg[i2]++;
        outdeg[i1]++;
    }
    /* ---- Topological Sort ---- */
    if (!TopologicalSort())
    {
        perror("Cycles exist. ");
        exit(0);
    }
    /* ---- Dp on Topological Sort ---- */
    int maxi = 0;
    for (int evid : topo)
    {
        Vertex &ev = event[evid];
        ev.EF = ev.ES + ev.DU;
        for (const Edge &eg : ev.to)
        {
            Vertex &evt = event[eg.to];
            if (ev.EF + eg.w > evt.ES)
            {
                evt.ES = ev.EF + eg.w;
                evt.ESpre = evid;
            }
        }
        if (ev.EF > event[maxi].EF)
            maxi = evid;
    }
    reverse(topo.begin(), topo.end());
    for (int evid : topo)
    {
        Vertex &ev = event[evid];
        if (outdeg[evid] == 0)
        {
            ev.LF = event[maxi].EF;
            ev.FF = 0;
        }
        ev.LS = ev.LF - ev.DU;
        ev.TF = ev.LS - ev.ES;
        if (ev.FF < 0)
            ev.FF = 0;
        for (const Edge &eg : ev.frm)
        {
            Vertex &evp = event[eg.to];
            // int ew = eg.w < 0 ? 0 : eg.w;
            int ew = eg.w;
            if (ev.LS - ew < evp.LF)
            {
                evp.LF = ev.LS - ew;
            }
            if (ev.ES - ew - evp.EF < evp.FF)
            {
                evp.FF = ev.ES - ew - evp.EF;
            }
        }
    }

    dispAll();
    getCriticalPath(maxi);
    /* ---- Display the Critical Path ---- */
    for (auto it = criticalpath.begin(), iend = criticalpath.end(); it != iend; it++)
    {
        printf("%c", event[*it].ID);
        if (next(it) != iend)
            printf("->");
        assert(event[*it].TF == 0 && event[*it].FF == 0);
    }
    return 0;
}
