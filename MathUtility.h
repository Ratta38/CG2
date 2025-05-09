#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

class MathUtility {
public:
	MathUtility();

	/// <summary>
	/// ベクトルの加算
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>加算結果</returns>
	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// ベクトルの減算
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>減算結果</returns>
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// ベクトルのスカラー倍
	/// </summary>
	/// <param name="scalar">スカラー</param>
	/// <param name="v">ベクトル</param>
	/// <returns>スカラー倍した値</returns>
	static Vector3 Multiply(float scalar, const Vector3& v);

	/// <summary>
	/// ベクトルの内積
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>内積</returns>
	static float Dot(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// ベクトルの長さ
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>長さ</returns>
	static float Length(const Vector3& v);

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>正規化した値</returns>
	static Vector3 Normalize(const Vector3& v);

	/// <summary>
	/// 拡大縮小行列
	/// </summary>
	/// <param name="scale">大きさ</param>
	/// <returns>拡大縮小行列</returns>
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	/// <summary>
	/// X軸周りの回転行列
	/// </summary>
	/// <param name="theta">角度X</param>
	/// <returns>X軸周りの回転行列</returns>
	static Matrix4x4 MakePitchRotateMatrix(float radian);

	/// <summary>
	/// Y軸周りの回転行列
	/// </summary>
	/// <param name="theta">角度Y</param>
	/// <returns>Y軸周りの回転行列</returns>
	static Matrix4x4 MakeYawRotateMatrix(float radian);

	/// <summary>
	/// Z軸周りの回転行列
	/// </summary>
	/// <param name="theta">角度Z</param>
	/// <returns>Z軸周りの回転行列</returns>
	static Matrix4x4 MakeRollRotateMatrix(float radian);

	/// <summary>
	/// 平行移動行列
	/// </summary>
	/// <param name="translate">座標</param>
	/// <returns>平行移動行列</returns>
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 Transpose(const Matrix4x4& m);
	static Matrix4x4 MakeIdentity4x4();

	/// <summary>
	/// 行列同士の積を求める
	/// </summary>
	/// <param name="matrix1">行列1</param>
	/// <param name="matrix2">行列2</param>
	/// <returns></returns>
	static Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

	/// <summary>
	/// アフィン行列
	/// </summary>
	/// <param name="scale">大きさ</param>
	/// <param name="rotate">角度</param>
	/// <param name="translate">座標</param>
	/// <returns>アフィン行列</returns>
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	/// <summary>
	/// 逆行列
	/// </summary>
	/// <param name="matrix">行列</param>
	/// <returns>逆行列</returns>
	static Matrix4x4 Inverse(const Matrix4x4& matrix);

	/// <summary>
	/// スクリーン座標系へ変換
	/// </summary>
	/// <param name="vector">ベクトル</param>
	/// <param name="matrix">行列</param>
	/// <returns>変換後の座標</returns>
	static Vector3 Transform(Vector3& vector, Matrix4x4& matrix);

	/// <summary>
	/// 正射影行列
	/// </summary>
	/// <param name="left">左端</param>
	/// <param name="top">上端</param>
	/// <param name="right">右端</param>
	/// <param name="bottom">下端</param>
	/// <param name="nearClip">近平面への距離</param>
	/// <param name="farClip">遠平面への距離</param>
	/// <returns>正規化デバイス座標系</returns>
	static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	/// <summary>
	/// 透視投影行列（同次クリップへの変換）
	/// </summary>
	/// <param name="fovY">画角Y</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">近平面への距離</param>
	/// <param name="farClip">遠平面への距離</param>
	/// <returns>正規化デバイス座標系</returns>
	static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	/// <summary>
	/// ビューポート変換（スクリーン座標系への変換）
	/// </summary>
	/// <param name="left">左</param>
	/// <param name="top">上</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="minD">最小深度値</param>
	/// <param name="maxD">最大深度値</param>
	/// <returns>スクリーン座標系</returns>
	static Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD);
};
