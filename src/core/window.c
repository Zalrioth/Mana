#include "core/window.h"

void new_window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit()) {
        printf("Error initializing GLFW!\n");
        exit(1);
    }

    window = glfwCreateWindow(640, 480, "Grindstone", NULL, NULL);
    if (!window) {
        glfwTerminate();
        printf("Error creating GLFW window!\n");
        exit(1);
    }

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);

    targetFps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
    limitDrawFPS = 1.0 / targetFps;
    fps = (int)targetFps;
    lastTime = get_time();
    timer = lastTime;
    for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
        fpsPast[loopNum] = fps;

    vector_init(&entities);
}

bool should_close()
{
    return glfwWindowShouldClose(window);
}

double get_time()
{
    struct timespec currentTime;
    timespec_get(&currentTime, TIME_UTC);

    return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000;
}

void update()
{
    nowTime = get_time();
    deltaTime += (nowTime - lastTime) / limitUpdateFPS;
    lastTime = nowTime;

    while (deltaTime >= 1.0) {
        logic(deltaTime / 20);
        updates++;
        deltaTime--;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    frames++;

    if (get_time() - timer > 1.0) {
        timer++;

        printf("Target FPS: %lf\n", targetFps);
        printf("Average FPS: %lf\n", averageFps);
        printf("Draw FPS: %d\n", frames);
        printf("Update FPS: %d\n\n", updates);

        fps = frames;

        float averageCalc = 0;
        for (int loopNum = FPS_COUNT - 1; loopNum >= 0; loopNum--) {
            if (loopNum != 0)
                fpsPast[loopNum] = fpsPast[loopNum - 1];

            averageCalc += fpsPast[loopNum];
        }
        fpsPast[0] = fps;
        averageFps = averageCalc / FPS_COUNT;

        updates = 0, frames = 0;
    }
}

void logic(double deltaTime)
{
    int i;

    VECTOR_ADD(entities, "Bonjour");
    VECTOR_ADD(entities, "tout");
    VECTOR_ADD(entities, "le");
    VECTOR_ADD(entities, "monde");

    for (i = 0; i < VECTOR_TOTAL(entities); i++)
        printf("%s ", VECTOR_GET(entities, char*, i));
    printf("\n");

    VECTOR_DELETE(entities, 3);
    VECTOR_DELETE(entities, 2);
    VECTOR_DELETE(entities, 1);

    VECTOR_SET(entities, 0, "Hello");
    VECTOR_ADD(entities, "World");

    for (i = 0; i < VECTOR_TOTAL(entities); i++)
        printf("%s ", VECTOR_GET(entities, char*, i));
    printf("\n");

    printf("before vector size: %d\n", VECTOR_TOTAL(entities));

    for (i = VECTOR_TOTAL(entities) - 1; i >= 0; i--)
        VECTOR_DELETE(entities, i);

    printf("total vector size: %d\n", VECTOR_TOTAL(entities));

    //VECTOR_FREE(entities);

    display();
}

void close()
{
    glfwTerminate();
}

/* code for model stuff
private:
    int ourShader;
    Model* ourModel = nullptr;
    glm::vec3* position = new glm::vec3(0.0, 0.0, 0.0);
    glm::vec3* scale = new glm::vec3(1.0, 1.0, 1.0);

public:
    ModelEntity(const char* vertex, const char* fragment, const char* model);
    ~ModelEntity();
    void render(EngineSettings* engineSettings);
    void update(EngineSettings* engineSettings);
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
};

ModelEntity::ModelEntity(const char* vertex, const char* fragment, const char* model)
{
    this->ourShader = createShader(vertex, fragment);
    this->ourModel = new Model(model);
}

ModelEntity::~ModelEntity()
{
    delete ourModel;
}

void ModelEntity::setPosition(float x, float y, float z)
{
    this->position->x = x;
    this->position->y = y;
    this->position->z = z;
}

void ModelEntity::setScale(float x, float y, float z)
{
    this->scale->x = x;
    this->scale->y = y;
    this->scale->z = z;
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

void ModelEntity::render(EngineSettings* engineSettings)
{
    use(this->ourShader);
    setMat4(this->ourShader, "projection", engineSettings->projectionMatrix);
    setMat4(this->ourShader, "view", engineSettings->viewMatrix);

    // glm::mat4 modelMatrix;
    glm::mat4 modelMatrix = aiMatrix4x4ToGlm(this->ourModel->rootMatrix);
    modelMatrix = glm::translate(modelMatrix, *this->position);
    modelMatrix = glm::scale(modelMatrix, *this->scale);
    setMat4(this->ourShader, "model", modelMatrix);
    this->ourModel->Draw(this->ourShader);
}
*/