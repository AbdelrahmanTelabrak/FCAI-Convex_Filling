#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include "iostream"
#include <windows.h>
#include "vector"
#include <utility>
#include "math.h"

using namespace std;

vector<POINT> points;
pair<int, int> Entry;
COLORREF color = RGB(255, 0, 0);

const int MAXENTRIES = 600;


void drawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    SetPixel(hdc, x1, y1, color);
    if (abs(dx) >= abs(dy)) {
        if (x1 > x2) {
            swap(x1, x2);
            swap(y1, y2);
        }
        int x = x1;
        double y = y1;
        double m = (double)dy / dx;
        while (x2 > x) {
            x++;
            y += m;
            SetPixel(hdc, x, round(y), color);
        }
    }
    else {
        if (y1 > y2) {
            swap(x1, x2);
            swap(y1, y2);
        }
        double x = x1;
        int y = y1;
        double mi = (double)dx / dy;
        while (y2 > y) {
            y++;
            x += mi;
            SetPixel(hdc, round(x), y, color);
        }
    }
}

void InitEntries(vector<pair<int, int>>& table)
{
    for(int i=0; i < MAXENTRIES;i++)
    {
        pair<int, int> e;e.first = MAXINT;e.second=-1;
        table.push_back(e);
    }
}

void ScanEdge(POINT v1,POINT v2,vector<pair<int, int>>& table)
{
    if(v1.y==v2.y)return;
    if(v1.y>v2.y)swap(v1,v2);
    double minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    double x=v1.x;
    int y=v1.y;
    while(y<v2.y)
    {
        if(x<table[y].first)
            table[y].first=(int)ceil(x);
        if(x>table[y].second)
            table[y].second=(int)floor(x);
        cout<<table[y].first<<" "<<table[y].second<<"\n";
        y++;
        x+=minv;
    }
}

void DrawSanLines(HDC hdc,vector<pair<int, int>> table,COLORREF color)
{
    for(int y=0;y<MAXENTRIES;y++)
        if(table[y].first<table[y].second)
            drawLine(hdc, table[y].first, y, table[y].second, y, color);
            /*
            for(int x=table[y].xmin;x<=table[y].xmax;x++)
                SetPixel(hdc,x,y,color);
                */



}

void ConvexFill(HDC hdc,vector<POINT> p,int n,COLORREF color)
{
    vector<pair<int, int>> table;
    InitEntries(table);
    POINT v1=p[n-1];
    for(int i=0;i<n;i++)
    {
        cout<<"in loop\n";
        POINT v2=p[i];
        ScanEdge(v1,v2,table);
        v1=p[i];
    }
    DrawSanLines(hdc,table,color);
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszArgument,
                   int nCmdShow) {
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
            0,                   /* Extended possibilites for variation */
            szClassName,         /* Classname */
            _T("Code::Blocks Template Windows App"),       /* Title Text */
            WS_OVERLAPPEDWINDOW, /* default window */
            CW_USEDEFAULT,       /* Windows decides the position */
            CW_USEDEFAULT,       /* where the window ends up on the screen */
            800,                 /* The programs width */
            600,                 /* and height in pixels */
            HWND_DESKTOP,        /* The window is a child-window to desktop */
            NULL,                /* No menu */
            hThisInstance,       /* Program Instance handler */
            NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0)) {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDOWN: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            points.push_back(pt);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }

        case WM_RBUTTONDOWN: {
            HDC hdc = GetDC(hwnd);
            int n = points.size();
            if (n > 2)  // fill the polygon when there are more than two points
            {
                ConvexFill(hdc, points, n, color);  // pass the address of the first element in the vector
            }
            //InvalidateRect(hwnd, NULL, TRUE);
            break;
        }

        case WM_KEYDOWN: {
            cout<<"Points are cleared\n";
            points.clear();
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
