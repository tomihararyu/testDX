#include "DxLib.h"
#include <cmath>
#include <cstdio>

const int screenWidth = 1920;//画面X
const int screenHeight = 1080;//画面Y
const int paddleWidth = 12;//パドルの幅
const int paddleHeight = 100;//パドルの高さ
const double minSpeed = 10.0;//最低速度
const double maxSpeed = 30.0;//最高速度
const double enemySpeed = 11.0; //敵パドルの追従速度

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    if (DxLib_Init() == -1) { return -1; }
    SetGraphMode(screenWidth, screenHeight, 32);//画面サイズ

    //色設定
    int ballColor = GetColor(255, 255, 255);
    int textColor = GetColor(255, 255, 255);

    //初期設定
    double ballX = screenWidth / 2.0;
    double ballY = screenHeight / 2.0;
    double ballSpeed = 10.0;
    double ballAngle = 20.0 * DX_PI / 180.0;
    const int ballRadius = 10;

    //パドル設定
    double padlX = screenWidth - paddleWidth - 12;
    double padlY = screenHeight / 2.0 - paddleHeight / 2.0;
    double enemyPadX = 12;
    double enemyPadY = screenHeight / 2.0 - paddleHeight / 2.0;

    //壁接触回数
    int leftWallCount = 0;
    int rightWallCount = 0;

    //メインループ
    while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        ballX += ballSpeed * std::cos(ballAngle);
        ballY += ballSpeed * std::sin(ballAngle);

        int mouseY;
        GetMousePoint(0, &mouseY);
        padlY = mouseY - paddleHeight / 2;

        double targetY = ballY - paddleHeight / 2;//CPUが作れなかったのでついてくるだけ
        if (enemyPadY < targetY) {
            enemyPadY += enemySpeed;
            if (enemyPadY > targetY) enemyPadY = targetY;
        }
        else if (enemyPadY > targetY) {
            enemyPadY -= enemySpeed;
            if (enemyPadY < targetY) enemyPadY = targetY;
        }

        //壁衝突判定
        if (ballX - ballRadius <= 0) {//右
            ballAngle = DX_PI - ballAngle;
            leftWallCount++;
            ballX = ballRadius;
        }
        if (ballX + ballRadius >= screenWidth) {//左
            ballAngle = DX_PI - ballAngle;
            rightWallCount++;
            ballX = screenWidth - ballRadius;
        }
        //上下壁
        if (ballY - ballRadius <= 0 || ballY + ballRadius >= screenHeight) {
            ballAngle = -ballAngle;
        }

        if (ballX + ballRadius >= padlX &&
            ballX - ballRadius < padlX + paddleWidth &&
            ballY + ballRadius >= padlY &&
            ballY - ballRadius <= padlY + paddleHeight)
        {
            //右パドル衝突処理
            double relativeIntersectY = (padlY + paddleHeight / 2.0) - ballY;
            double normalizedRelativeIntersectY = relativeIntersectY / (paddleHeight / 2.0);

            double reflectAngle = normalizedRelativeIntersectY * (45.0 * DX_PI / 180.0);
            ballAngle = DX_PI - reflectAngle;

            ballSpeed = minSpeed + (maxSpeed - minSpeed) * fabs(normalizedRelativeIntersectY);
            ballX = padlX - ballRadius;
        }

            //左パドル衝突判定
        if (ballX - ballRadius <= enemyPadX + paddleWidth &&
            ballX + ballRadius > enemyPadX &&
            ballY + ballRadius >= enemyPadY &&
            ballY - ballRadius <= enemyPadY + paddleHeight)
        {
            //左パドル衝突処理
            double relativeIntersectY = (enemyPadY + paddleHeight / 2.0) - ballY;
            double normalizedRelativeIntersectY = relativeIntersectY / (paddleHeight / 2.0);

            double reflectAngle = normalizedRelativeIntersectY * (45.0 * DX_PI / 180.0);
            ballAngle = reflectAngle;

            ballSpeed = minSpeed + (maxSpeed - minSpeed) * fabs(normalizedRelativeIntersectY);
            ballX = enemyPadX + paddleWidth + ballRadius;
        }

        //描画
        DrawBox(enemyPadX, enemyPadY, enemyPadX + paddleWidth, enemyPadY + paddleHeight, ballColor, TRUE);
        DrawBox(padlX, padlY, padlX + paddleWidth, padlY + paddleHeight, ballColor, TRUE);
        DrawCircle(static_cast<int>(ballX), static_cast<int>(ballY), ballRadius, ballColor, TRUE);

        //表示
        DrawFormatString(screenWidth / 4, 50, textColor, "%d", leftWallCount);
        DrawFormatString(3 * screenWidth / 4, 50, textColor, "%d", rightWallCount);
        ScreenFlip();
        WaitTimer(16);
        if (ProcessMessage() == -1) break;
        if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
    }

    DxLib_End();
    return 0;
}
