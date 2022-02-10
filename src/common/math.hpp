
struct Vec2 {
public:
    float x, y;

    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(Vec2 rhs);
    Vec2 operator-(Vec2 rhs);
    Vec2 operator*(float rhs);
    Vec2 operator/(float rhs);

    float mag();
    float dot(Vec2 other);
};
