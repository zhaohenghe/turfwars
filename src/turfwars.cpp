#include <iostream>
#include <sstream>
#include <iomanip>
#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include "ECS.hpp"
#include "BufferedRaylib.hpp"

size_t globalComponentCounter = 0;

template<typename T>
concept Transformer = requires(T t, raylib::Matrix m)
{
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, auto transformer)
{
    raylib::Matrix backup = model.transform;                    // Stores the current transformation matrix of the model in a variable, `backup`
    model.transform = transformer(backup);                      // Calls the transformer function, passing in the current transform into `backup`
    model.Draw({});        
    model.GetTransformedBoundingBox().Draw(raylib::RAYWHITE);   // Draws the bounding box of the model
    model.transform = backup;                                   // Restores the model’s original transformation matrix from the backup
}

void DrawModel(raylib::Model& model, auto transformer)
{
    raylib::Matrix backup = model.transform;                    
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

struct TransformComponent
{
    raylib::Vector3 position = {0, 0, 0};
    float heading = 0.0f;
};

struct RenderComponent
{
    raylib::Model* model = nullptr;
    bool showBoundingBox = false;
    bool isRocket = false;

    void ToggleBoundingBox()
    {
        showBoundingBox = !showBoundingBox;
    }
};

void RenderSystem(cs381::Scene<cs381::ComponentStorage>& scene, float dt)
{
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); ++e)
    {
        if (!scene.HasComponent<TransformComponent>(e)) continue;
        if (!scene.HasComponent<RenderComponent>(e)) continue;

        auto& transform = scene.GetComponent<TransformComponent>(e);
        auto& render = scene.GetComponent<RenderComponent>(e);

        if (render.model == nullptr) continue;

        if (render.showBoundingBox)
        {
            DrawBoundedModel(*render.model, [&](raylib::Matrix& matrix)
            {
                if (render.isRocket)
                {
                    return matrix.Translate(transform.position).RotateZ(raylib::Degree(transform.heading));
                }
                else
                {
                    return matrix.Translate(transform.position).RotateY(raylib::Degree(transform.heading) + raylib::Degree(90));
                }
            });
        }
        else
        {
            DrawModel(*render.model, [&](raylib::Matrix& matrix)
            {
                if (render.isRocket)
                {
                    return matrix.Translate(transform.position).RotateZ(raylib::Degree(transform.heading));
                }
                else
                {
                    return matrix.Translate(transform.position).RotateY(raylib::Degree(transform.heading) + raylib::Degree(90));
                }
            });
        }
    }
}

struct KinematicsComponent
{
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f}; 
    float speed = 0.0f;
    float targetSpeed = 0.0f;      
    float acceleration = 0.0f;                    
    float maxSpeed = 0.0f; 
    float timeOnGrass = 0.0f;                 

    void AdjustSpeed(bool increase)
    {
        if (increase && speed < maxSpeed)
        {
            targetSpeed += acceleration;
        }
        else if (!increase)
        {
            targetSpeed -= acceleration;
        }
    }
};

void KinematicsSystem(cs381::Scene<cs381::ComponentStorage>& scene, float dt, bool& gameRunning)
{
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); ++e)
    {
        if (!scene.HasComponent<TransformComponent>(e)) continue;
        if (!scene.HasComponent<KinematicsComponent>(e)) continue;

        auto& transform = scene.GetComponent<TransformComponent>(e);
        auto& kinematics = scene.GetComponent<KinematicsComponent>(e);

        if (kinematics.targetSpeed < kinematics.maxSpeed)
        {
            kinematics.targetSpeed += kinematics.acceleration * dt;
        }

        kinematics.speed = std::lerp(kinematics.speed, kinematics.targetSpeed, dt);

        transform.position.x += kinematics.velocity.x * dt;
        transform.position.y += kinematics.velocity.y * dt;
        transform.position.z += kinematics.velocity.z * dt;

        if (std::abs(transform.position.x) > 50 || std::abs(transform.position.z) > 50)
        {
            std::cout << "Entity " << e << " is out of bounds!" << std::endl;
            gameRunning = false;
            return;
        }
    }
}

void GrassTrackingSystem(cs381::Scene<cs381::ComponentStorage>& scene, cs381::Entity selectedEntity, float dt)
{
    if (!scene.HasComponent<TransformComponent>(selectedEntity)) return;
    if (!scene.HasComponent<KinematicsComponent>(selectedEntity)) return;

    auto& transform = scene.GetComponent<TransformComponent>(selectedEntity);
    auto& kinematics = scene.GetComponent<KinematicsComponent>(selectedEntity);


    bool isOnGrass = (transform.position.x > -50 && transform.position.x < 50 &&
                      transform.position.z > -50 && transform.position.z < 50);

    if (isOnGrass)
    {
        kinematics.timeOnGrass += dt; 
    }
}

struct Physics2DComponent
{
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f};
    float heading = 0.0f;        
    float targetHeading = 0.0f;  
    float turnRate = 0.0f;       
    float currentRotation = 0.0f; 

    void AdjustHeading(bool left)
    {
        targetHeading += (left ? 1 : -1) * turnRate;
    }
};

void Physics2DSystem(cs381::Scene<cs381::ComponentStorage>& scene, float dt)
{
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); ++e)
    {
        if (!scene.HasComponent<TransformComponent>(e)) continue;
        if (!scene.HasComponent<Physics2DComponent>(e)) continue;
        if (!scene.HasComponent<KinematicsComponent>(e)) continue;

        auto& transform = scene.GetComponent<TransformComponent>(e);
        auto& physics2D = scene.GetComponent<Physics2DComponent>(e);
        auto& kinematics = scene.GetComponent<KinematicsComponent>(e);

        physics2D.velocity.x = cos(transform.heading * DEG2RAD) * kinematics.speed;
        physics2D.velocity.z = -sin(transform.heading * DEG2RAD) * kinematics.speed;
        
        transform.position.x += physics2D.velocity.x * dt;
        transform.position.z += physics2D.velocity.z * dt;

        physics2D.currentRotation = transform.heading;
        physics2D.currentRotation = std::lerp(physics2D.currentRotation, physics2D.targetHeading, dt);
        transform.heading = physics2D.currentRotation;
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    // window setup
    int screenWidth = 800;
    int screenHeight = 600;
    raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    bool gameRunning = true;

    // scene objects
    int modelSize = 3;

    auto sedan = raylib::Model("meshes/sedan.glb");    
    sedan.transform = raylib::Matrix::Identity().Scale(modelSize);
    auto delivery = raylib::Model("meshes/delivery.glb");    
    delivery.transform = raylib::Matrix::Identity().Scale(modelSize);
    auto raceCar = raylib::Model("meshes/race.glb");    
    raceCar.transform = raylib::Matrix::Identity().Scale(modelSize);
    auto suv = raylib::Model("meshes/suv.glb");    
    suv.transform = raylib::Matrix::Identity().Scale(modelSize);
    auto taxi = raylib::Model("meshes/taxi.glb");    
    taxi.transform = raylib::Matrix::Identity().Scale(modelSize);
    auto rocket = raylib::Model("meshes/rocketA.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(modelSize);


    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    // camera setup
    auto camera = raylib::Camera({0, 30, -60}, 
                                {0, -10, 0}, 
                                {0, 1, 0}, 
                                90, 
                                CAMERA_PERSPECTIVE);

    // skybox setup
    cs381::SkyBox sky("textures/skybox.png");

    // audio setup
    raylib::AudioDevice audio;
    raylib::Music wind;
    wind.Load("../assets/audio/wind.mp3");
    wind.SetVolume(0.7f);
    wind.SetLooping(true);
    wind.Play();

    // scene
    cs381::Scene<cs381::ComponentStorage> scene;

    auto sedan1 = scene.CreateEntity();
    auto taxi1 = scene.CreateEntity();
    auto raceCar1 = scene.CreateEntity();

    scene.AddComponent<TransformComponent>(sedan1) = {{-20, 0, -10}, 0.0f};
    scene.AddComponent<TransformComponent>(taxi1) = {{-20, 0, -15}, 0.0f};
    scene.AddComponent<TransformComponent>(raceCar1) = {{-20, 0, -20}, 0.0f};

    scene.AddComponent<RenderComponent>(sedan1) = {&sedan, true, false};
    scene.AddComponent<RenderComponent>(taxi1) = {&taxi, false, false};
    scene.AddComponent<RenderComponent>(raceCar1) = {&raceCar, false, false};

    scene.AddComponent<KinematicsComponent>(sedan1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 3.0f, 100.0f};
    scene.AddComponent<KinematicsComponent>(taxi1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 3.0f, 100.0f};
    scene.AddComponent<KinematicsComponent>(raceCar1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 3.0f, 100.0f};

    scene.AddComponent<Physics2DComponent>(sedan1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 7.0f, 0.0f};
    scene.AddComponent<Physics2DComponent>(taxi1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 8.0f, 0.0f};
    scene.AddComponent<Physics2DComponent>(raceCar1) = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 10.0f, 0.0f};

    // buffred input setup
    raylib::BufferedInput input;

    cs381::Entity selectedEntity = sedan1;

    input["select"] = raylib::Action::button(raylib::Button::key(KEY_TAB));
    input["select"].AddCallback([&scene, &selectedEntity](float state, float change) 
    {
        if (state == 1) 
        {
            if (scene.HasComponent<RenderComponent>(selectedEntity)) 
            {
                scene.GetComponent<RenderComponent>(selectedEntity).showBoundingBox = false;
            }

            selectedEntity = (selectedEntity + 1) % scene.entityMasks.size();

            if (scene.HasComponent<RenderComponent>(selectedEntity)) 
            {
                scene.GetComponent<RenderComponent>(selectedEntity).showBoundingBox = true;
            }
        }
    });

    input["move_forward"] = raylib::Action::button(raylib::Button::key(KEY_W));
    input["move_forward"].AddCallback([&scene, &selectedEntity](float state, float change) 
    {
        if (state == 1 && scene.HasComponent<KinematicsComponent>(selectedEntity)) 
        {
            scene.GetComponent<KinematicsComponent>(selectedEntity).AdjustSpeed(true);
        }
    });

    input["move_backward"] = raylib::Action::button(raylib::Button::key(KEY_S));
    input["move_backward"].AddCallback([&scene, &selectedEntity](float state, float change) 
    {
        if (state == 1 && scene.HasComponent<KinematicsComponent>(selectedEntity)) 
        {
            scene.GetComponent<KinematicsComponent>(selectedEntity).AdjustSpeed(false);
        }
    });

    input["turn_left"] = raylib::Action::button(raylib::Button::key(KEY_A));
    input["turn_left"].AddCallback([&scene, &selectedEntity](float state, float change) 
    {
        if (state == 1 && scene.HasComponent<Physics2DComponent>(selectedEntity)) 
        {
            scene.GetComponent<Physics2DComponent>(selectedEntity).AdjustHeading(true);
        }
    });

    input["turn_right"] = raylib::Action::button(raylib::Button::key(KEY_D));
    input["turn_right"].AddCallback([&scene, &selectedEntity](float state, float change) 
    {
        if (state == 1 && scene.HasComponent<Physics2DComponent>(selectedEntity)) 
        {
            scene.GetComponent<Physics2DComponent>(selectedEntity).AdjustHeading(false);
        }
    });
    
    while (!window.ShouldClose())
    {
        auto& kinematics = scene.GetComponent<KinematicsComponent>(selectedEntity);

        if (gameRunning)
        {
            input.PollEvents();
            window.BeginDrawing();
            wind.Update();
            {
                window.ClearBackground(WHITE);
                camera.BeginMode();
                    sky.Draw();
                    grass.Draw({});

                    auto dt = window.GetFrameTime();
                    RenderSystem(scene, dt);
                    KinematicsSystem(scene, dt, gameRunning);
                    Physics2DSystem(scene, dt);
                    GrassTrackingSystem(scene, selectedEntity, dt);

                camera.EndMode();

                raylib::DrawText(("FPS: " + std::to_string(window.GetFPS())).c_str(), 10, 10, 20, GREEN);

                if (scene.HasComponent<KinematicsComponent>(selectedEntity))
                {
                    std::string label = "Time on Grass: ";
                    std::ostringstream timeStream;
                    timeStream << std::fixed << std::setprecision(3) << std::setw(6) << kinematics.timeOnGrass;
                    std::string timeText = label + timeStream.str();
                    int labelWidth = MeasureText(label.c_str(), 20);
                    int numberWidth = MeasureText("000.000", 20);
                    raylib::DrawText(label.c_str(), screenWidth - labelWidth - numberWidth - 10, 10, 20, BLUE);
                    raylib::DrawText(timeStream.str().c_str(), screenWidth - numberWidth - 10, 10, 20, BLUE);
                }
            }
            window.EndDrawing();
        }
        else
        {
            window.BeginDrawing();
            {
                window.ClearBackground(BLACK);
                std::string gameOverText = "Game Over! Time on Grass (sec): " + std::to_string(kinematics.timeOnGrass);
                raylib::DrawText(gameOverText.c_str(), screenWidth / 2 - MeasureText(gameOverText.c_str(), 20) / 2, screenHeight / 2 - 10, 20, WHITE);
                raylib::DrawText("Press ESC to Exit", screenWidth / 2 - MeasureText("Press ESC to Exit", 20) / 2, screenHeight / 2 + 20, 20, RED);
            }
            window.EndDrawing();
        }
    }
    return 0;
}