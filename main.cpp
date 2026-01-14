#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 700;

// Cấu trúc cho ngôi sao
struct Star {
    float x, y;
    float size;
    float brightness;
    float speed;
    float pulse;
    sf::Color color;
    bool isShooting;
    float shootSpeed;
    float trailLength;
};

// Cấu trúc cho hiệu ứng particle
struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    float size;
    sf::Color color;
};

// Cấu trúc cho floating heart
struct FloatingHeart {
    float x, y;
    float size;
    float speed;
    float angle;
    float rotation;
    float rotationSpeed;
    float pulse;
    sf::Color color;
};

// Biến toàn cục
std::vector<Star> stars;
std::vector<Particle> particles;
std::vector<FloatingHeart> floatingHearts;
sf::Clock starClock;
sf::Clock animationClock;
sf::Font font;
bool showText = true;
float textAlpha = 255.0f;
bool mousePressed = false;
sf::Vector2f lastMousePos;

// Biến cho emoji di chuyển
sf::Vector2f emojiPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
sf::Vector2f emojiVelocity(0.0f, 0.0f);
const float EMOJI_SPEED = 300.0f;
const float FRICTION = 0.92f;
bool emojiDragging = false;
sf::Vector2f dragOffset;

// BIẾN MỚI: Quản lý xoay 3D
float faceRotationX = 0.0f;    // Xoay quanh trục X (gật/lắc)
float faceRotationY = 0.0f;    // Xoay quanh trục Y (quay trái/phải)
float targetRotationX = 0.0f;  // Mục tiêu xoay X
float targetRotationY = 0.0f;  // Mục tiêu xoay Y
const float ROTATION_SPEED = 8.0f;     // Tốc độ xoay
const float MAX_ROTATION_ANGLE = 25.0f; // Góc xoay tối đa (độ)

// Biến cho hiệu ứng di chuột không kéo emoji
sf::Vector2f lastMouseWorldPos(0, 0);
bool mouseOverEmoji = false;

// Hàm tạo màu ngẫu nhiên
sf::Color randomColor() {
    int r = 50 + rand() % 206;
    int g = 50 + rand() % 206;
    int b = 50 + rand() % 206;
    return sf::Color(r, g, b);
}

// Hàm tạo màu pastel
sf::Color randomPastelColor() {
    int r = 180 + rand() % 55;
    int g = 180 + rand() % 55;
    int b = 180 + rand() % 55;
    return sf::Color(r, g, b);
}

// Hàm tạo particle
void createParticle(float x, float y, sf::Color color, int count = 1, float speedMultiplier = 1.0f) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = (rand() % 100 - 50) / 50.0f * speedMultiplier;
        p.vy = (rand() % 100 - 50) / 50.0f * speedMultiplier;
        p.life = 1.0f;
        p.maxLife = 0.3f + (rand() % 100) / 100.0f * 1.0f;
        p.size = 1.0f + (rand() % 100) / 100.0f * 3.0f;
        p.color = color;
        particles.push_back(p);
    }
}

// Hàm tạo floating heart
void createFloatingHeart(float x, float y, sf::Color color = sf::Color::Transparent) {
    FloatingHeart heart;
    heart.x = x;
    heart.y = y;
    heart.size = 0.8f + (rand() % 100) / 100.0f * 1.5f;
    heart.speed = 0.8f + (rand() % 100) / 100.0f * 1.5f;
    heart.angle = (rand() % 100) / 100.0f * 2.0f * M_PI;
    heart.rotation = 0.0f;
    heart.rotationSpeed = (rand() % 100 - 50) / 100.0f * 2.0f;
    heart.pulse = (rand() % 100) / 100.0f * 2.0f * M_PI;

    if (color == sf::Color::Transparent) {
        int colorChoice = rand() % 6;
        switch(colorChoice) {
            case 0: heart.color = sf::Color(255, 48, 84); break;
            case 1: heart.color = sf::Color(255, 100, 150); break;
            case 2: heart.color = sf::Color(255, 150, 50); break;
            case 3: heart.color = sf::Color(255, 100, 200); break;
            case 4: heart.color = sf::Color(255, 80, 100); break;
            case 5: heart.color = sf::Color(255, 180, 80); break;
        }
    } else {
        heart.color = color;
    }

    floatingHearts.push_back(heart);
}

// Khởi tạo các ngôi sao
void initStars() {
    srand(static_cast<unsigned>(time(nullptr)));
    stars.clear();

    for (int i = 0; i < 350; i++) {
        Star star;
        star.x = static_cast<float>(rand() % WINDOW_WIDTH);
        star.y = static_cast<float>(rand() % WINDOW_HEIGHT);
        star.size = 0.2f + static_cast<float>(rand() % 100) / 100.0f * 2.0f;
        star.brightness = 0.3f + static_cast<float>(rand() % 70) / 100.0f;
        star.speed = 0.5f + static_cast<float>(rand() % 100) / 100.0f * 2.5f;
        star.pulse = static_cast<float>(rand() % 100) / 100.0f * 2.0f * M_PI;
        star.isShooting = (rand() % 100) < 3;
        star.shootSpeed = 0.0f;
        star.trailLength = 0.0f;

        if (star.isShooting) {
            star.shootSpeed = 80.0f + static_cast<float>(rand() % 150);
            star.trailLength = 15.0f + static_cast<float>(rand() % 30);
        }

        int colorChoice = rand() % 100;
        if (colorChoice < 50) {
            star.color = sf::Color::White;
        } else if (colorChoice < 70) {
            star.color = sf::Color(255, 255, 180);
        } else if (colorChoice < 85) {
            star.color = sf::Color(180, 220, 255);
        } else if (colorChoice < 95) {
            star.color = sf::Color(255, 200, 200);
        } else {
            star.color = sf::Color(200, 255, 200);
        }

        stars.push_back(star);
    }
}

// Cập nhật trạng thái các ngôi sao
void updateStars(float deltaTime) {
    float time = starClock.getElapsedTime().asSeconds();

    for (auto& star : stars) {
        star.brightness = 0.3f + 0.7f * (sin(time * star.speed + star.pulse) * 0.5f + 0.5f);

        if (star.isShooting) {
            star.x += star.shootSpeed * deltaTime * cos(star.pulse);
            star.y += star.shootSpeed * deltaTime * sin(star.pulse);

            if (star.x < -100 || star.x > WINDOW_WIDTH + 100 ||
                star.y < -100 || star.y > WINDOW_HEIGHT + 100) {
                star.x = static_cast<float>(rand() % WINDOW_WIDTH);
                star.y = static_cast<float>(rand() % WINDOW_HEIGHT);
                star.shootSpeed = 80.0f + static_cast<float>(rand() % 150);
                star.pulse = static_cast<float>(rand() % 100) / 100.0f * 2.0f * M_PI;
            }
        } else {
            star.x += sin(time * 0.3f + star.pulse) * 0.15f;
            star.y += cos(time * 0.2f + star.pulse) * 0.1f;
        }

        if (!star.isShooting) {
            if (star.x < 0) star.x += WINDOW_WIDTH;
            if (star.x > WINDOW_WIDTH) star.x -= WINDOW_WIDTH;
            if (star.y < 0) star.y += WINDOW_HEIGHT;
            if (star.y > WINDOW_HEIGHT) star.y -= WINDOW_HEIGHT;
        }
    }
}

// Cập nhật particles
void updateParticles(float deltaTime) {
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->x += it->vx * deltaTime * 60.0f;
        it->y += it->vy * deltaTime * 60.0f;
        it->vy += 0.08f * deltaTime * 60.0f;
        it->life -= deltaTime * 1.5f;

        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

// Cập nhật floating hearts
void updateFloatingHearts(float deltaTime) {
    float time = animationClock.getElapsedTime().asSeconds();

    for (auto it = floatingHearts.begin(); it != floatingHearts.end(); ) {
        it->x += cos(it->angle) * it->speed * deltaTime * 25.0f;
        it->y -= it->speed * deltaTime * 25.0f;
        it->rotation += it->rotationSpeed * deltaTime * 25.0f;
        it->angle += sin(time + it->pulse) * 0.03f;

        float pulseFactor = 0.95f + 0.1f * sin(time * 2.0f + it->pulse);
        it->size = it->size * pulseFactor;

        if (it->y < -50 || it->x < -50 || it->x > WINDOW_WIDTH + 50) {
            it = floatingHearts.erase(it);
        } else {
            ++it;
        }
    }
}

// Cập nhật emoji di chuyển VÀ XOAY 3D
void updateEmoji(float deltaTime, sf::Vector2f mouseWorldPos) {
    emojiVelocity *= FRICTION;
    emojiPosition += emojiVelocity * deltaTime;

    float faceRadius = 150.0f;
    if (emojiPosition.x < faceRadius) {
        emojiPosition.x = faceRadius;
        emojiVelocity.x = -emojiVelocity.x * 0.5f;
    }
    if (emojiPosition.x > WINDOW_WIDTH - faceRadius) {
        emojiPosition.x = WINDOW_WIDTH - faceRadius;
        emojiVelocity.x = -emojiVelocity.x * 0.5f;
    }
    if (emojiPosition.y < faceRadius) {
        emojiPosition.y = faceRadius;
        emojiVelocity.y = -emojiVelocity.y * 0.5f;
    }
    if (emojiPosition.y > WINDOW_HEIGHT - faceRadius) {
        emojiPosition.y = WINDOW_HEIGHT - faceRadius;
        emojiVelocity.y = -emojiVelocity.y * 0.5f;
    }

    // CẬP NHẬT XOAY 3D DỰA TRÊN DI CHUYỂN CHUỘT
    if (!emojiDragging) {
        // Tính khoảng cách từ chuột đến tâm emoji
        float dx = mouseWorldPos.x - emojiPosition.x;
        float dy = mouseWorldPos.y - emojiPosition.y;
        float distance = sqrt(dx * dx + dy * dy);

        // Kiểm tra chuột có ở trên emoji không
        mouseOverEmoji = (distance <= faceRadius);

        if (mouseOverEmoji && distance > 10.0f) {
            // Tính góc xoay dựa trên vị trí chuột so với tâm emoji
            // Chuột bên phải -> xoay sang trái (âm), chuột bên trái -> xoay sang phải (dương)
            float normalizedX = dx / faceRadius;
            float normalizedY = dy / faceRadius;

            // Giới hạn giá trị
            normalizedX = std::max(-1.0f, std::min(1.0f, normalizedX));
            normalizedY = std::max(-1.0f, std::min(1.0f, normalizedY));

            // Tính góc xoay mục tiêu
            targetRotationY = -normalizedX * MAX_ROTATION_ANGLE; // Xoay trái/phải
            targetRotationX = normalizedY * MAX_ROTATION_ANGLE;  // Xoay lên/xuống
        } else {
            // Từ từ trở về vị trí ban đầu
            targetRotationX *= 0.9f;
            targetRotationY *= 0.9f;
        }
    } else {
        // Khi đang kéo emoji, xoay theo hướng di chuyển
        if (emojiVelocity.x != 0 || emojiVelocity.y != 0) {
            float speed = sqrt(emojiVelocity.x * emojiVelocity.x + emojiVelocity.y * emojiVelocity.y);
            float speedFactor = std::min(1.0f, speed / EMOJI_SPEED);

            targetRotationY = -(emojiVelocity.x / EMOJI_SPEED) * MAX_ROTATION_ANGLE * speedFactor;
            targetRotationX = (emojiVelocity.y / EMOJI_SPEED) * MAX_ROTATION_ANGLE * speedFactor;
        } else {
            targetRotationX *= 0.9f;
            targetRotationY *= 0.9f;
        }
    }

    // Làm mượt chuyển động xoay
    float diffX = targetRotationX - faceRotationX;
    float diffY = targetRotationY - faceRotationY;

    faceRotationX += diffX * ROTATION_SPEED * deltaTime;
    faceRotationY += diffY * ROTATION_SPEED * deltaTime;

    // Giới hạn góc xoay
    faceRotationX = std::max(-MAX_ROTATION_ANGLE, std::min(MAX_ROTATION_ANGLE, faceRotationX));
    faceRotationY = std::max(-MAX_ROTATION_ANGLE, std::min(MAX_ROTATION_ANGLE, faceRotationY));

    // Tạo hiệu ứng particle khi di chuyển nhanh
    if (emojiVelocity.x != 0 || emojiVelocity.y != 0) {
        float speed = sqrt(emojiVelocity.x * emojiVelocity.x + emojiVelocity.y * emojiVelocity.y);
        if (speed > 50.0f && rand() % 100 < 30) {
            createParticle(emojiPosition.x, emojiPosition.y,
                          sf::Color(255, 219, 89, 150), 1, 0.5f);
        }
    }
}

// Vẽ các ngôi sao
void drawStars(sf::RenderWindow& window) {
    float time = starClock.getElapsedTime().asSeconds();

    for (const auto& star : stars) {
        if (star.isShooting) {
            sf::VertexArray trail(sf::TriangleStrip);
            float trailAlpha = star.brightness * 120.0f;

            for (int i = 0; i < 4; i++) {
                float trailPos = static_cast<float>(i) / 3.0f;
                float trailX = star.x - cos(star.pulse) * star.trailLength * trailPos;
                float trailY = star.y - sin(star.pulse) * star.trailLength * trailPos;
                float trailSize = star.size * (1.0f - trailPos * 0.7f);

                sf::Color trailColor = star.color;
                trailColor.a = static_cast<sf::Uint8>(trailAlpha * (1.0f - trailPos));

                trail.append(sf::Vertex(
                    sf::Vector2f(trailX - sin(star.pulse) * trailSize,
                                trailY + cos(star.pulse) * trailSize),
                    trailColor
                ));

                trail.append(sf::Vertex(
                    sf::Vector2f(trailX + sin(star.pulse) * trailSize,
                                trailY - cos(star.pulse) * trailSize),
                    trailColor
                ));
            }

            window.draw(trail);
        }

        sf::CircleShape starShape(star.size);
        starShape.setPosition(star.x - star.size, star.y - star.size);

        sf::Color starColor = star.color;
        starColor.a = static_cast<sf::Uint8>(star.brightness * 255);
        starShape.setFillColor(starColor);

        window.draw(starShape);

        if (star.size > 1.0f) {
            float pulseSize = star.size * (1.3f + 0.2f * sin(time * 2.0f + star.pulse));
            sf::CircleShape glow(pulseSize);
            glow.setPosition(star.x - pulseSize, star.y - pulseSize);
            sf::Color glowColor = star.color;
            glowColor.a = static_cast<sf::Uint8>(star.brightness * 60);
            glow.setFillColor(glowColor);
            window.draw(glow);
        }
    }
}

// Vẽ particles
void drawParticles(sf::RenderWindow& window) {
    for (const auto& p : particles) {
        float lifeRatio = p.life / p.maxLife;
        sf::CircleShape particle(p.size * lifeRatio);
        particle.setPosition(p.x, p.y);

        sf::Color particleColor = p.color;
        particleColor.a = static_cast<sf::Uint8>(255 * lifeRatio);
        particle.setFillColor(particleColor);

        window.draw(particle);
    }
}

// Hàm vẽ hình trái tim
void drawHeart(sf::RenderWindow& window, float x, float y, float size,
               sf::Color fillColor, float rotationAngle = 0.0f,
               bool withOutline = true, float pulseFactor = 1.0f) {

    float angleRad = rotationAngle * M_PI / 180.0f;
    size *= pulseFactor;

    sf::ConvexShape heartFill;
    heartFill.setPointCount(30);

    sf::Color centerColor = fillColor;
    sf::Color edgeColor = sf::Color(
        std::min(255, fillColor.r - 30),
        std::min(255, fillColor.g - 20),
        std::min(255, fillColor.b - 10)
    );

    for (int i = 0; i < 30; i++) {
        float t = static_cast<float>(i) / 29.0f * 2.0f * M_PI;
        float px_orig = 16 * pow(sin(t), 3) * size;
        float py_orig = -(13 * cos(t) - 5 * cos(2*t) - 2 * cos(3*t) - cos(4*t)) * size;

        float px = x + px_orig * cos(angleRad) - py_orig * sin(angleRad);
        float py = y + px_orig * sin(angleRad) + py_orig * cos(angleRad);

        heartFill.setPoint(i, sf::Vector2f(px, py));

        float dist = sqrt(px_orig*px_orig + py_orig*py_orig) / (16.0f * size);
        sf::Color pointColor = sf::Color(
            centerColor.r + static_cast<int>((edgeColor.r - centerColor.r) * dist),
            centerColor.g + static_cast<int>((edgeColor.g - centerColor.g) * dist),
            centerColor.b + static_cast<int>((edgeColor.b - centerColor.b) * dist)
        );

        heartFill.setFillColor(pointColor);
    }

    window.draw(heartFill);

    if (withOutline) {
        sf::VertexArray heartOutline(sf::LineStrip);

        for (int i = 0; i <= 30; i++) {
            float t = static_cast<float>(i % 30) / 29.0f * 2.0f * M_PI;
            float px_orig = 16 * pow(sin(t), 3) * size;
            float py_orig = -(13 * cos(t) - 5 * cos(2*t) - 2 * cos(3*t) - cos(4*t)) * size;

            float px = x + px_orig * cos(angleRad) - py_orig * sin(angleRad);
            float py = y + px_orig * sin(angleRad) + py_orig * cos(angleRad);

            heartOutline.append(sf::Vertex(
                sf::Vector2f(px, py),
                sf::Color(30, 30, 30, 150)
            ));
        }

        window.draw(heartOutline);
    }
}

// Vẽ floating hearts
void drawFloatingHearts(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();

    for (const auto& heart : floatingHearts) {
        float pulse = 1.0f + 0.15f * sin(time * 2.0f + heart.pulse);
        drawHeart(window, heart.x, heart.y, heart.size,
                 heart.color, heart.rotation, true, pulse);
    }
}

// Hàm vẽ đường cong dày VỚI XOAY 3D
void drawThickCurve(sf::RenderWindow& window, float centerX, float centerY,
                   float width, float height, int numPoints, sf::Color color,
                   bool upward = true, float thickness = 3.0f,
                   float rotationX = 0.0f, float rotationY = 0.0f,
                   float offsetX = 0.0f, float offsetY = 0.0f) {

    sf::VertexArray curve(sf::TriangleStrip);

    // Chuyển đổi góc từ độ sang radian
    float angleX = rotationX * M_PI / 180.0f;
    float angleY = rotationY * M_PI / 180.0f;

    for (int i = 0; i <= numPoints; i++) {
        float t = static_cast<float>(i) / numPoints;
        float x = -width/2 + width * t + offsetX; // Tọa độ địa phương
        float y = offsetY + (upward ? -1 : 1) * height * sin(t * M_PI);

        // Áp dụng xoay 3D (đơn giản hóa - chỉ dùng perspective)
        // Hiệu ứng 3D: xoay quanh trục Y làm thay đổi tọa độ X
        float rotatedX = x * cos(angleY) - y * sin(angleX) * sin(angleY);
        float rotatedY = y * cos(angleX);

        // Đưa về tọa độ thế giới
        float worldX = centerX + rotatedX;
        float worldY = centerY + rotatedY;

        // Tính vector pháp tuyến cho độ dày
        float nx = -sin(angleY);
        float ny = cos(angleX);

        curve.append(sf::Vertex(
            sf::Vector2f(worldX - nx * thickness / 2, worldY - ny * thickness / 2),
            color
        ));

        curve.append(sf::Vertex(
            sf::Vector2f(worldX + nx * thickness / 2, worldY + ny * thickness / 2),
            color
        ));
    }

    window.draw(curve);
}

// Vẽ emoji mặt cười với 3 trái tim và XOAY 3D
void drawEmoji(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    float faceRadius = 150.0f;

    float speed = sqrt(emojiVelocity.x * emojiVelocity.x + emojiVelocity.y * emojiVelocity.y);
    float speedPulse = 1.0f + 0.05f * (speed / EMOJI_SPEED);
    float timePulse = 1.0f + 0.03f * sin(time * 2.0f);
    float pulseFactor = speedPulse * timePulse;

    // Tính hệ số co giãn do xoay 3D
    float scaleX = 1.0f - abs(faceRotationY) / (MAX_ROTATION_ANGLE * 2.0f) * 0.2f;
    float scaleY = 1.0f - abs(faceRotationX) / (MAX_ROTATION_ANGLE * 2.0f) * 0.1f;

    float effectiveFaceRadius = faceRadius * pulseFactor * std::min(scaleX, scaleY);

    // Bóng đổ với hiệu ứng 3D
    float shadowOffsetX = 30.0f + abs(faceRotationY) * 0.5f;
    float shadowOffsetY = 8.0f + abs(faceRotationX) * 0.3f;

    sf::CircleShape faceShadow(effectiveFaceRadius);
    faceShadow.setPosition(emojiPosition.x - effectiveFaceRadius + shadowOffsetX,
                          emojiPosition.y - effectiveFaceRadius + shadowOffsetY);
    faceShadow.setFillColor(sf::Color(0, 0, 0, 40 - abs(faceRotationX + faceRotationY)));
    window.draw(faceShadow);

    // Tạo transform cho xoay 3D
    sf::Transform transform;
    transform.translate(emojiPosition);
    transform.rotate(faceRotationY, 0, 0); // Xoay quanh trục Y
    transform.rotate(faceRotationX, 0, 0); // Xoay quanh trục X
    transform.scale(scaleX, scaleY);
    transform.translate(-emojiPosition);

    // Khuôn mặt với xoay 3D
    sf::CircleShape face(effectiveFaceRadius);
    face.setPosition(emojiPosition.x - effectiveFaceRadius,
                    emojiPosition.y - effectiveFaceRadius);
    face.setFillColor(sf::Color(255, 170, 60));
    face.setOutlineThickness(4.0f);
    face.setOutlineColor(sf::Color(40, 40, 40, 220));
    window.draw(face, transform);

    // HIGHLIGHT CÙNG TÂM VỚI KHUÔN MẶT
    float highlightSize = effectiveFaceRadius * 0.8f;
    sf::CircleShape highlight(highlightSize);
    highlight.setPosition(emojiPosition.x - highlightSize,
                         emojiPosition.y - highlightSize);
    highlight.setFillColor(sf::Color(255, 255, 255, 80));

    // Tạo transform riêng cho highlight để nó xoay cùng khuôn mặt
    sf::Transform highlightTransform;
    highlightTransform.translate(emojiPosition);
    highlightTransform.rotate(faceRotationY * 0.7f, 0, 0); // Xoay ít hơn để tự nhiên
    highlightTransform.rotate(faceRotationX * 0.7f, 0, 0);
    highlightTransform.translate(-emojiPosition);

    window.draw(highlight, highlightTransform);

    // Mắt với hiệu ứng 3D
    float eyeWidth = 40.0f;
    float eyeHeight = 15.0f;
    float eyePulse = 1.0f + 0.1f * sin(time * 5.0f) * (speed / EMOJI_SPEED);

    // Vẽ mắt với hiệu ứng 3D
    drawThickCurve(window, emojiPosition.x, emojiPosition.y,
                  eyeWidth * eyePulse, eyeHeight,
                  20, sf::Color(40, 40, 40, 220), true, 4.0f,
                  faceRotationX, faceRotationY, -60, -40);

    drawThickCurve(window, emojiPosition.x, emojiPosition.y,
                  eyeWidth * eyePulse, eyeHeight,
                  20, sf::Color(40, 40, 40, 220), true, 4.0f,
                  faceRotationX, faceRotationY, 40, -40);

    // Highlight mắt với hiệu ứng 3D
    sf::CircleShape eyeHighlight1(3);
    eyeHighlight1.setPosition(emojiPosition.x - 55, emojiPosition.y - 50);
    eyeHighlight1.setFillColor(sf::Color(255, 255, 255, 200));
    window.draw(eyeHighlight1, transform);

    sf::CircleShape eyeHighlight2(3);
    eyeHighlight2.setPosition(emojiPosition.x + 45, emojiPosition.y - 50);
    eyeHighlight2.setFillColor(sf::Color(255, 255, 255, 200));
    window.draw(eyeHighlight2, transform);

    // Miệng với hiệu ứng 3D
    float smileWidth = 120.0f * (1.0f + 0.2f * (speed / EMOJI_SPEED));
    float smileHeight = 40.0f;

    drawThickCurve(window, emojiPosition.x, emojiPosition.y,
                  smileWidth, smileHeight,
                  30, sf::Color(40, 40, 40, 220), false, 5.0f,
                  faceRotationX, faceRotationY, 0, 30);

    // 3 trái tim bay xung quanh (cập nhật vị trí với xoay 3D)
    float heartSize = 2.5f;
    float heartPulse = 1.0f + 0.15f * sin(time * 3.0f);
    float orbitAngle = time * 0.5f;

    // Tính offset cho trái tim dựa trên góc xoay
    float heartOffsetX = faceRotationY * 0.5f;
    float heartOffsetY = faceRotationX * 0.3f;

    // Trái tim 1
    float heart1X = emojiPosition.x + 120 + 20 * sin(time + orbitAngle) + heartOffsetX;
    float heart1Y = emojiPosition.y - 90 + 10 * cos(time + orbitAngle) + heartOffsetY;
    // Thêm xoay cho trái tim dựa trên xoay khuôn mặt
    float heart1Rotation = 20.0f + sin(time) * 10.0f + faceRotationY * 0.5f;
    drawHeart(window, heart1X, heart1Y,
             heartSize, sf::Color(255, 48, 84),
             heart1Rotation, true, heartPulse);

    // Trái tim 2
    float heart2X = emojiPosition.x - 140 + 15 * sin(time * 1.5f + orbitAngle) - heartOffsetX;
    float heart2Y = emojiPosition.y + 40 + 8 * cos(time * 1.5f + orbitAngle) + heartOffsetY;
    float heart2Rotation = 10.0f + sin(time * 1.2f) * 8.0f - faceRotationY * 0.3f;
    drawHeart(window, heart2X, heart2Y,
             heartSize * 1.1f, sf::Color(255, 48, 84),
             heart2Rotation, true, heartPulse);

    // Trái tim 3
    float heart3X = emojiPosition.x + 130 + 10 * sin(time * 0.8f + orbitAngle) + heartOffsetX;
    float heart3Y = emojiPosition.y + 70 + 12 * cos(time * 0.8f + orbitAngle) + heartOffsetY;
    float heart3Rotation = -5.0f + sin(time * 0.9f) * 12.0f + faceRotationY * 0.4f;
    drawHeart(window, heart3X, heart3Y,
             heartSize * 1.1f, sf::Color(255, 48, 84),
             heart3Rotation, true, heartPulse);

    // Hào quang khi di chuyển nhanh (cũng xoay)
    if (speed > 100.0f) {
        float haloSize = effectiveFaceRadius * 1.5f * (1.0f + 0.1f * sin(time * 10.0f));
        sf::CircleShape halo(haloSize);
        halo.setPosition(emojiPosition.x - haloSize, emojiPosition.y - haloSize);
        halo.setFillColor(sf::Color(255, 219, 89, static_cast<sf::Uint8>(30 * (speed / EMOJI_SPEED))));
        window.draw(halo, transform);
    }
}

// Vẽ text
void drawText(sf::RenderWindow& window, float deltaTime) {
    static sf::Clock textClock;
    float time = textClock.getElapsedTime().asSeconds();

    if (showText) {
        // Kiểm tra xem font có load được không
        static bool fontAvailable = true;

        if (fontAvailable) {
            // Tiêu đề
            sf::Text title("Interactive Apple Emoji", font, 36);
            title.setPosition(WINDOW_WIDTH / 2 - title.getLocalBounds().width / 2, 20);
            title.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(textAlpha)));
            title.setStyle(sf::Text::Bold);
            window.draw(title);

            // Hiệu ứng nhấp nháy
            float blinkAlpha = 150 + 105 * sin(time * 2.5f);
            sf::Color textColor(255, 255, 255, static_cast<sf::Uint8>(blinkAlpha));

            // Hướng dẫn - vẽ từng dòng
            std::vector<std::string> controls = {
                "CONTROLS:",
                "Drag emoji to move",
                "Move mouse over emoji to rotate 3D",
                "Click to create hearts",
                "SPACE: Explosion",
                "R: Reset stars",
                "C: Clear hearts",
                "T: Toggle text",
                "ESC: Exit"
            };

            for (size_t i = 0; i < controls.size(); i++) {
                sf::Text line(controls[i], font, 18);
                line.setPosition(20, 80 + i * 26);
                line.setFillColor(textColor);
                window.draw(line);
            }

            // Thông tin emoji
            std::vector<std::string> info = {
                "Emoji Pos: (" + std::to_string((int)emojiPosition.x) + "," +
                                 std::to_string((int)emojiPosition.y) + ")",
                "Speed: " + std::to_string((int)sqrt(emojiVelocity.x*emojiVelocity.x +
                                                     emojiVelocity.y*emojiVelocity.y)),
                "Rotation X: " + std::to_string((int)faceRotationX) + "°",
                "Rotation Y: " + std::to_string((int)faceRotationY) + "°",
                "Hearts: " + std::to_string(floatingHearts.size()),
                "Particles: " + std::to_string(particles.size())
            };

            for (size_t i = 0; i < info.size(); i++) {
                sf::Text line(info[i], font, 16);
                line.setPosition(WINDOW_WIDTH - 250, 80 + i * 22);
                line.setFillColor(sf::Color(200, 230, 255, 200));
                window.draw(line);
            }

            // Trạng thái
            if (emojiDragging) {
                sf::Text dragging("DRAGGING EMOJI", font, 24);
                dragging.setPosition(WINDOW_WIDTH / 2 - dragging.getLocalBounds().width / 2,
                                   WINDOW_HEIGHT - 100);
                dragging.setFillColor(sf::Color(255, 100, 100, 200));
                dragging.setStyle(sf::Text::Bold);
                window.draw(dragging);
            } else if (mouseOverEmoji) {
                sf::Text rotating("3D ROTATION ACTIVE", font, 24);
                rotating.setPosition(WINDOW_WIDTH / 2 - rotating.getLocalBounds().width / 2,
                                   WINDOW_HEIGHT - 100);
                rotating.setFillColor(sf::Color(100, 200, 255, 200));
                rotating.setStyle(sf::Text::Bold);
                window.draw(rotating);
            }
        } else {
            // Nếu không có font, hiển thị thông báo đơn giản
            sf::Text noFont("Font not available - Controls disabled", font, 20);
            noFont.setPosition(WINDOW_WIDTH / 2 - 200, 20);
            noFont.setFillColor(sf::Color(255, 100, 100, 200));
            window.draw(noFont);
        }
    }
}

int main() {
    // Tạo cửa sổ
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                           "Interactive Apple Emoji with 3D Rotation",
                           sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    // Load font
    if (!font.loadFromFile("fonts/arial.ttf")) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cout << "Không tìm thấy font. Chương trình vẫn chạy nhưng không hiển thị text." << std::endl;
            showText = false;
        }
    }

    // Khởi tạo
    initStars();

    // Tạo nền gradient
    sf::VertexArray nightSky(sf::Quads, 8);

    // Layer 1
    nightSky[0].position = sf::Vector2f(0, 0);
    nightSky[1].position = sf::Vector2f(WINDOW_WIDTH, 0);
    nightSky[2].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    nightSky[3].position = sf::Vector2f(0, WINDOW_HEIGHT);

    nightSky[0].color = sf::Color(10, 5, 30);
    nightSky[1].color = sf::Color(15, 10, 40);
    nightSky[2].color = sf::Color(5, 10, 35);
    nightSky[3].color = sf::Color(8, 5, 25);

    // Layer 2
    nightSky[4].position = sf::Vector2f(0, WINDOW_HEIGHT * 0.6f);
    nightSky[5].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT * 0.6f);
    nightSky[6].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    nightSky[7].position = sf::Vector2f(0, WINDOW_HEIGHT);

    nightSky[4].color = sf::Color(5, 10, 35, 150);
    nightSky[5].color = sf::Color(10, 15, 45, 150);
    nightSky[6].color = sf::Color(2, 5, 20, 200);
    nightSky[7].color = sf::Color(4, 8, 28, 200);

    // Tạo vài floating hearts ban đầu
    for (int i = 0; i < 8; i++) {
        createFloatingHeart(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
    }

    sf::Clock frameClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }

            // Xử lý kéo và thả emoji
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    float faceRadius = 150.0f;

                    float distance = sqrt(pow(mousePos.x - emojiPosition.x, 2) +
                                         pow(mousePos.y - emojiPosition.y, 2));

                    if (distance <= faceRadius) {
                        emojiDragging = true;
                        dragOffset = emojiPosition - mousePos;
                        createParticle(emojiPosition.x, emojiPosition.y,
                                      sf::Color(255, 219, 89), 15, 2.0f);
                    } else {
                        mousePressed = true;
                        lastMousePos = mousePos;
                        createParticle(mousePos.x, mousePos.y,
                                      sf::Color(255, 100 + rand() % 155, 100 + rand() % 155), 10);
                        createFloatingHeart(mousePos.x, mousePos.y);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mousePressed = false;

                    if (emojiDragging) {
                        emojiDragging = false;
                        sf::Vector2f currentPos(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f dragDistance = currentPos - (emojiPosition - dragOffset);
                        emojiVelocity = dragDistance * 5.0f;

                        createParticle(emojiPosition.x, emojiPosition.y,
                                      sf::Color(255, 200, 100), 20, 1.5f);

                        for (int i = 0; i < 3; i++) {
                            createFloatingHeart(emojiPosition.x, emojiPosition.y,
                                              sf::Color(255, 48 + rand() % 100, 84 + rand() % 100));
                        }
                    }
                }
            }

            // Xử lý di chuyển chuột
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                lastMouseWorldPos = mousePos;

                if (emojiDragging) {
                    emojiPosition = mousePos + dragOffset;

                    if (rand() % 100 < 40) {
                        createParticle(emojiPosition.x, emojiPosition.y,
                                      sf::Color(255, 219, 89, 100), 1, 0.3f);
                    }
                } else if (mousePressed) {
                    sf::Vector2f currentPos(event.mouseMove.x, event.mouseMove.y);
                    float distance = sqrt(pow(currentPos.x - lastMousePos.x, 2) +
                                         pow(currentPos.y - lastMousePos.y, 2));

                    if (distance > 5.0f) {
                        for (int i = 0; i < 3; i++) {
                            float t = static_cast<float>(i) / 3.0f;
                            float x = lastMousePos.x + (currentPos.x - lastMousePos.x) * t;
                            float y = lastMousePos.y + (currentPos.y - lastMousePos.y) * t;
                            createParticle(x, y, randomPastelColor(), 1, 0.5f);
                        }
                        lastMousePos = currentPos;
                    }
                }
            }

            // Xử lý phím
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    for (int i = 0; i < 100; i++) {
                        createParticle(emojiPosition.x, emojiPosition.y, randomColor(), 1, 3.0f);
                    }
                    for (int i = 0; i < 15; i++) {
                        createFloatingHeart(emojiPosition.x, emojiPosition.y);
                    }
                }
                if (event.key.code == sf::Keyboard::T) {
                    showText = !showText;
                }
                if (event.key.code == sf::Keyboard::C) {
                    floatingHearts.clear();
                }
                if (event.key.code == sf::Keyboard::R) {
                    initStars();
                }
                if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
                    emojiVelocity.y -= EMOJI_SPEED * 0.5f;
                }
                if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
                    emojiVelocity.y += EMOJI_SPEED * 0.5f;
                }
                if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                    emojiVelocity.x -= EMOJI_SPEED * 0.5f;
                }
                if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                    emojiVelocity.x += EMOJI_SPEED * 0.5f;
                }
            }
        }

        float deltaTime = frameClock.restart().asSeconds();

        // Cập nhật
        updateStars(deltaTime);
        updateParticles(deltaTime);
        updateFloatingHearts(deltaTime);
        updateEmoji(deltaTime, lastMouseWorldPos);

        // Hiệu ứng text fade
        if (textAlpha > 0 && !showText) {
            textAlpha -= deltaTime * 150.0f;
            if (textAlpha < 0) textAlpha = 0;
        } else if (textAlpha < 255 && showText) {
            textAlpha += deltaTime * 150.0f;
            if (textAlpha > 255) textAlpha = 255;
        }

        // Vẽ
        window.clear();
        window.draw(nightSky);
        drawStars(window);
        drawParticles(window);
        drawFloatingHearts(window);
        drawEmoji(window);
        drawText(window, deltaTime);

        window.display();
    }

    return 0;
}
