// https://learnopengl.com/Getting-started/Camera

#include <bits/stdc++.h>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_3<CGAL::Color, K> Vb;
typedef CGAL::Triangulation_data_structure_3<Vb> Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds> Delaunay;
typedef Delaunay::Edge Delaunay_edge;
typedef Delaunay::Point Point;

typedef Delaunay::Point   Point;
typedef glm::vec3 Vector3D;
typedef Delaunay::Simplex Simplex;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void init();
void render();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLuint VBO;
GLuint VAO;
int cnt;
int cntRayvertices = 0;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float Scale = 10.0;

GLfloat vertices[3 * 5000000], Rayvertices[3 * 10000];



using namespace std;

map<pair<Point, Point>, int> Map;

struct Triangle {
    Vector3D vertex0, vertex1, vertex2;

    Triangle(Vector3D _vertex0, Vector3D _vertex1, Vector3D _vertex2) {
        vertex0 = _vertex0;
        vertex1 = _vertex1;
        vertex2 = _vertex2;
    }
};

vector<Triangle> sTriangle;

pair<Vector3D, Vector3D> Edge[3 * 500000];

Vector3D Pointtovec3(Point P) {
    Vector3D vecresult;
    vecresult.x = P.x();
    vecresult.y = P.y();
    vecresult.z = P.z();
    return vecresult;
}

Point vec3toPoint(Vector3D Vec3) {
    return Point( Vec3.x, Vec3.y, Vec3.z);
}

bool cmp(Vector3D V1, Vector3D V2) {
    const float EPSILON = 1e-7;
    if (abs(V1.x - V2.x) > EPSILON || abs(V1.y - V2.y) > EPSILON || abs(V1.z - V2.z) > EPSILON) return false;
    return true;
}


bool RayIntersectsTriangle(Vector3D rayOrigin,
                           Vector3D rayVector,
                           Triangle inTriangle,
                           Vector3D& outIntersectionPoint)
{
    const float EPSILON = 0.0000001;
    Vector3D vertex0 = inTriangle.vertex0;
    Vector3D vertex1 = inTriangle.vertex1;
    Vector3D vertex2 = inTriangle.vertex2;
    Vector3D edge1, edge2, edge3, h, s, q;
    float a,f,u,v;

    cout<<fixed<<setprecision(7);

    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    edge3 = vertex2 - vertex1;

    if (cmp(normalize(edge1) , rayVector) || cmp(normalize(edge1) , -rayVector)) return true;
    if (cmp(normalize(edge2) , rayVector) || cmp(normalize(edge2) , -rayVector)) return true;
    if (cmp(normalize(edge3) , rayVector) || cmp(normalize(edge3) , -rayVector)) return true;

    a = dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1/a;
    s = rayOrigin - vertex0;
    u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;


    q = cross(s, edge1);
    v = f * dot(rayVector, q);


    if (v < 0.0 || u + v > 1.0)
        return false;

    float t = f * dot(edge2, q);

    return true;
}




bool CheckTriangle(Triangle triangle) {
    Point P1 = vec3toPoint(triangle.vertex0);
    Point P2 = vec3toPoint(triangle.vertex1);
    Point P3 = vec3toPoint(triangle.vertex2);


    if (Map.find(make_pair(P1, P2)) == Map.end()) return false;
    if (Map.find(make_pair(P2, P3)) == Map.end()) return false;
    if (Map.find(make_pair(P1, P3)) == Map.end()) return false;
    return true;
}

int main()
{
    // C++11 random lib
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(-10.0, 10.0);


    ifstream iFileT("inp1.txt",ios::in);

    int nPoint;
    iFileT >> nPoint;


    list<Point> L;
    //nPoint = 4;
    for (int i = 0; i < nPoint; i ++) {
        float x, y, z, rbg, abc ;
        //iFileT>> x >> y >> z ;
        iFileT>> x >> y >> z >> rbg >> abc;
        //x = dis(gen); y = dis(gen); z = dis(gen);


        L.push_back(Point(x*Scale, y*Scale, z*Scale));
    }

    iFileT.close();



    ofstream oFileT("output.txt",ios::out);



    double time1 = 1000 * clock() / CLOCKS_PER_SEC;
    Delaunay T(L.begin(), L.end());;
    float time2 = 1000 * clock() / CLOCKS_PER_SEC;

    Delaunay::Triangulation_3::Finite_edges_iterator eit = T.finite_edges_begin();
    Delaunay::Triangulation_3::Finite_edges_iterator end = T.finite_edges_end();

    Point p;
    Point q;
    ++eit;

    // Edge lists -> output.txt
    int cntP = 0;


    for(;eit != end; ++eit)
    {
        Delaunay_edge edge = *eit;
        q = edge.first->vertex(edge.third)->point();
        p = edge.first->vertex(edge.second)->point();
        /*vertices[cnt++] = p.x();
        vertices[cnt++] = p.y();
        vertices[cnt++] = p.z();
        vertices[cnt++] = q.x();
        vertices[cnt++] = q.y();
        vertices[cnt++] = q.z();*/

        ++cntP;
        Vector3D pp = Pointtovec3(p);
        Vector3D qq = Pointtovec3(q);


        Map[make_pair(p,q)] = cntP;
        Map[make_pair(q,p)] = cntP;


        oFileT << '(' << p << ") - (" << q << ')' << endl;
    }

    float focal_length = 10.0;


    Delaunay::Triangulation_3::Finite_facets_iterator fit = T.finite_facets_begin();

    int cntTriangle = 0;
    int cntEdge = 0;
    for(;fit != T.finite_facets_end(); ++fit) {
        Vector3D vertex0 = Pointtovec3(fit->first->vertex( (fit->second+1)%4)->point() );
        Vector3D vertex1 = Pointtovec3(fit->first->vertex( (fit->second+2)%4)->point() );
        Vector3D vertex2 = Pointtovec3(fit->first->vertex( (fit->second+3)%4)->point() );
        Triangle NewTriangle = Triangle(vertex0, vertex1, vertex2);


        if (CheckTriangle(NewTriangle) ) {

            ++cntTriangle;
            sTriangle.push_back( NewTriangle );

            Edge[cntEdge++] = make_pair(vertex0, vertex1);
            Edge[cntEdge++] = make_pair(vertex0, vertex2);
            Edge[cntEdge++] = make_pair(vertex1, vertex2);
        }
    }

    //cout<< cntTriangle <<endl;


    int dem = 0;
    Vector3D ORay = Vector3D(0.0, 0.0, 0.0);
    Vector3D RayO, RayT;


    for(int j = 0; j < 100; ++j) {
        RayO = Vector3D(dis(gen), dis(gen), dis(gen));

        RayO = RayO - ORay;
        RayT = normalize(RayO);

        int ccntTriangle = 0;

        if (j)
        for(int i = 0; i < cntTriangle; ++i)
            if (CheckTriangle(sTriangle[i])) sTriangle[ccntTriangle++] = sTriangle[i];

        cntTriangle = ccntTriangle;

        for(int i = 0; i < cntTriangle; ++i)
        if (CheckTriangle(sTriangle[i])) {
            Vector3D IntersectionPoint;
            if (RayIntersectsTriangle(RayO, RayT, sTriangle[i], IntersectionPoint) == true) {
                Point P1 = vec3toPoint( sTriangle[i].vertex0 );
                Point P2 = vec3toPoint( sTriangle[i].vertex1 );
                Point P3 = vec3toPoint( sTriangle[i].vertex2 );
                Map.erase(make_pair(P1, P2));
                Map.erase(make_pair(P2, P1));
                Map.erase(make_pair(P1, P3));
                Map.erase(make_pair(P3, P1));
                Map.erase(make_pair(P3, P2));
                Map.erase(make_pair(P2, P3));


            }
        }

    }


    map<pair<Point, Point>, int> ::iterator mit = Map.begin();

    for(; mit != Map.end(); ++mit) {
        Point q = mit->first.first;
        Point p = mit->first.second;


        vertices[cnt++] = p.x();
        vertices[cnt++] = p.y();
        vertices[cnt++] = p.z();
        vertices[cnt++] = q.x();
        vertices[cnt++] = q.y();
        vertices[cnt++] = q.z();
    }



    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Delaunay Triangulation", NULL, NULL);

    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 800, 800);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader MyShader("vertex.glsl", "fragment.glsl");
    init();

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MyShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        MyShader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        MyShader.setMat4("view", view);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    //float time2 = 1000 * clock() / CLOCKS_PER_SEC;
    cerr<< time1 <<' '<< time2 <<' '<< time2 - time1 <<endl;
            // cerr << "\nTime elapsed: " << 1000 * clock() / CLOCKS_PER_SEC << "ms\n";
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 25.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void init(){
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(Rayvertices), Rayvertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

}

void render(){
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, cnt * 3);
    //glDrawArrays(GL_LINES, 0, cntRayvertices * 3);
    glBindVertexArray(0);
}

// Camera manipulation : https://learnopengl.com/Getting-started/Camera

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
