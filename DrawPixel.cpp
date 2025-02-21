#include "DxLib.h"
#include <cmath>
#include <cstdio>

const int screenWidth = 1920;//���X
const int screenHeight = 1080;//���Y
const int paddleWidth = 12;//�p�h���̕�
const int paddleHeight = 100;//�p�h���̍���
const double minSpeed = 10.0;//�Œᑬ�x
const double maxSpeed = 30.0;//�ō����x
const double enemySpeed = 11.0; //�G�p�h���̒Ǐ]���x

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    if (DxLib_Init() == -1) { return -1; }
    SetGraphMode(screenWidth, screenHeight, 32);//��ʃT�C�Y

    //�F�ݒ�
    int ballColor = GetColor(255, 255, 255);
    int textColor = GetColor(255, 255, 255);

    //�����ݒ�
    double ballX = screenWidth / 2.0;
    double ballY = screenHeight / 2.0;
    double ballSpeed = 10.0;
    double ballAngle = 20.0 * DX_PI / 180.0;
    const int ballRadius = 10;

    //�p�h���ݒ�
    double padlX = screenWidth - paddleWidth - 12;
    double padlY = screenHeight / 2.0 - paddleHeight / 2.0;
    double enemyPadX = 12;
    double enemyPadY = screenHeight / 2.0 - paddleHeight / 2.0;

    //�ǐڐG��
    int leftWallCount = 0;
    int rightWallCount = 0;

    //���C�����[�v
    while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        ballX += ballSpeed * std::cos(ballAngle);
        ballY += ballSpeed * std::sin(ballAngle);

        int mouseY;
        GetMousePoint(0, &mouseY);
        padlY = mouseY - paddleHeight / 2;

        double targetY = ballY - paddleHeight / 2;//CPU�����Ȃ������̂ł��Ă��邾��
        if (enemyPadY < targetY) {
            enemyPadY += enemySpeed;
            if (enemyPadY > targetY) enemyPadY = targetY;
        }
        else if (enemyPadY > targetY) {
            enemyPadY -= enemySpeed;
            if (enemyPadY < targetY) enemyPadY = targetY;
        }

        //�ǏՓ˔���
        if (ballX - ballRadius <= 0) {//�E
            ballAngle = DX_PI - ballAngle;
            leftWallCount++;
            ballX = ballRadius;
        }
        if (ballX + ballRadius >= screenWidth) {//��
            ballAngle = DX_PI - ballAngle;
            rightWallCount++;
            ballX = screenWidth - ballRadius;
        }
        //�㉺��
        if (ballY - ballRadius <= 0 || ballY + ballRadius >= screenHeight) {
            ballAngle = -ballAngle;
        }

        if (ballX + ballRadius >= padlX &&
            ballX - ballRadius < padlX + paddleWidth &&
            ballY + ballRadius >= padlY &&
            ballY - ballRadius <= padlY + paddleHeight)
        {
            //�E�p�h���Փˏ���
            double relativeIntersectY = (padlY + paddleHeight / 2.0) - ballY;
            double normalizedRelativeIntersectY = relativeIntersectY / (paddleHeight / 2.0);

            double reflectAngle = normalizedRelativeIntersectY * (45.0 * DX_PI / 180.0);
            ballAngle = DX_PI - reflectAngle;

            ballSpeed = minSpeed + (maxSpeed - minSpeed) * fabs(normalizedRelativeIntersectY);
            ballX = padlX - ballRadius;
        }

            //���p�h���Փ˔���
        if (ballX - ballRadius <= enemyPadX + paddleWidth &&
            ballX + ballRadius > enemyPadX &&
            ballY + ballRadius >= enemyPadY &&
            ballY - ballRadius <= enemyPadY + paddleHeight)
        {
            //���p�h���Փˏ���
            double relativeIntersectY = (enemyPadY + paddleHeight / 2.0) - ballY;
            double normalizedRelativeIntersectY = relativeIntersectY / (paddleHeight / 2.0);

            double reflectAngle = normalizedRelativeIntersectY * (45.0 * DX_PI / 180.0);
            ballAngle = reflectAngle;

            ballSpeed = minSpeed + (maxSpeed - minSpeed) * fabs(normalizedRelativeIntersectY);
            ballX = enemyPadX + paddleWidth + ballRadius;
        }

        //�`��
        DrawBox(enemyPadX, enemyPadY, enemyPadX + paddleWidth, enemyPadY + paddleHeight, ballColor, TRUE);
        DrawBox(padlX, padlY, padlX + paddleWidth, padlY + paddleHeight, ballColor, TRUE);
        DrawCircle(static_cast<int>(ballX), static_cast<int>(ballY), ballRadius, ballColor, TRUE);

        //�\��
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
