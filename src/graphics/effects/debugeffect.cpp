/*void renderCrossHair()
{
    glPushMatrix();
    glLoadIdentity();

    float inc = 0.05f;
    glLineWidth(2.0f);

    glBegin(GL_LINES);

    glColor3f(1.0f, 1.0f, 1.0f);

    // Horizontal line
    glVertex3f(-inc, 0.0f, 0.0f);
    glVertex3f(+inc, 0.0f, 0.0f);
    glEnd();

    // Vertical line
    glBegin(GL_LINES);
    glVertex3f(0.0f, -inc, 0.0f);
    glVertex3f(0.0f, +inc, 0.0f);
    glEnd();

    glPopMatrix();
}

void renderAxes(EngineSettings* engineSettings)
{
    glPushMatrix();
    glLoadIdentity();
    float rotX = engineSettings->camera->camera.getRotation().x;
    float rotY = camera.getRotation().y;
    float rotZ = 0;
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    // X Axis
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    // Y Axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // Z Axis
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    glPopMatrix();
}*/