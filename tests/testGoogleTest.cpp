// #include <gtest/gtest.h>
#include "gmock/gmock.h" // Brings in gMock.

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

class Turtle
{
public:
    // virtual ~Turtle() {}
    virtual void PenUp() = 0;
    virtual void PenDown() = 0;
    virtual void Forward(int distance) = 0;
    virtual void Turn(int degrees) = 0;
    virtual void GoTo(int x, int y) = 0;
    virtual int GetX() const = 0;
    virtual int GetY() const = 0;
};

class TurtleImpl : public Turtle
{
public:
    // virtual ~TurtleImpl() { ; }
    void PenUp() override { ; }
    void PenDown() override { ; }
    void Forward(int distance) override { ; }
    void Turn(int degrees) override { ; }
    void GoTo(int x, int y) override { ; }
    int GetX() const override { return 0; }
    int GetY() const override { return 0; }

public:
    TurtleImpl() { ; }
    void call() { PenDown(); }
};
class MockTurtle : public Turtle
{
public:
    MOCK_METHOD(void, PenUp, (), (override));
    MOCK_METHOD(void, PenDown, (), (override));
    MOCK_METHOD(void, Forward, (int distance), (override));
    MOCK_METHOD(void, Turn, (int degrees), (override));
    MOCK_METHOD(void, GoTo, (int x, int y), (override));
    MOCK_METHOD(int, GetX, (), (const, override));
    MOCK_METHOD(int, GetY, (), (const, override));
};

class Painter
{
    Turtle *turtle;

public:
    Painter(Turtle *turtle)
        : turtle(turtle) {}

    bool DrawCircle(int, int, int)
    {
        turtle->PenDown();
        return true;
    }
};
using ::testing::AtLeast; // #1
using ::testing::Return;
TEST(PainterTest, CanDrawSomething)
{
    MockTurtle turtle;             // #2
    EXPECT_CALL(turtle, PenDown()) // #3
        .Times(AtLeast(1));
    EXPECT_CALL(turtle, GetX())
        .WillRepeatedly(Return(300));

    // TurtleImpl tImpl;
    // tImpl.call();
    Painter painter(&turtle);
    EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
    EXPECT_EQ(300, turtle.GetX());
}

int main(int argc, char **argv)
{
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}