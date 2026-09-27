/*
 * Object T007 (data/tu_map.json), guessed original file Mat44.cpp.
 *   .text 0x4077f0-0x409e88 (no COMDATs)       .rdata 0x574320-0x574334: 2.0f, pad, double 0.01, 1e-6f
 * The float matrix layer, then the Mat44 tail 0x409827-0x409e87 (Transpose, Transpose3x3InPlace, Transpose3x3,
 * NormalizeColumnsInPlace, NormalizeColumns, TransformPoint, with the <d3dvec.inl> operator/ and Normalize they use).
 * The candidate object starts 0x407a20 and 0x408510 (data/tu_map.json) are undecided; this is the one-object reading.
 */
/* BYTES: slot-group, slot-name, view. */
/* BYTES(slot-group): Mat44::SetRotZYX, SetRotYZX, SetRotZXY, SetRotXZY, SetRotYXZ, SetRotXYZ: the Euler terms are one EulerTerms struct only to pin the original frame offsets */
/* BYTES(slot-group): Mat44::SetRotAxisAngleDiv, Mat44::SetRotAxisAngle: the terms are one QuatTerms struct only to pin the original frame offsets */
/* BYTES(view): Mat44_Mul, Mat44::MulScalar, Mat44::ScalarMul: the original returns the matrix by value: the explicit out pointer is the hidden return slot */
/*
 * The float matrix layer, SheepD3D.exe 0x4077f0-0x409826: Mat44, a D3DMATRIX (16 floats, m[row][col] = _11.._44, D3D's
 * row-vector convention v' = v * M, translation in row 3).
 *
 * The file is written against DirectX 7 idioms: sinf/cosf are the <math.h> inline float forms (VC6 defines them for C++ on
 * x86 as `(float)sin((double)x)`, which is why their results pass through a temporary), and Mat44_SetLookAt is the DX7 SDK's
 * D3DUtil_SetViewMatrix reworked, written with the D3DVECTOR operators of <d3dvec.inl> (inlined, so their temporaries show up
 * as stack slots). The vector type here is Vec3f, which has D3DVECTOR's layout.
 * The products that return a matrix by value (Mat44_Mul, _MulScalar, _ScalarMul) are written with an explicit result
 * pointer: the same machine code as `Mat44 operator*(...)` with its hidden return slot.
 *
 * Local frames: VC6 /Od does not lay locals out in declaration order. It files them in 16 buckets by a hash of the name
 * (32-bit h = (h << 2) + (h >> 4) + c over the characters, then h ^= h >> 16; bucket = h & 15), gives slots from -4
 * downward bucket by bucket, and within a bucket takes the most recently declared first. The names the original used are
 * unknown, so the Euler, quaternion and perspective builders pin the original's offsets with a struct (members in address
 * order, lowest first); Mat44_SetLookAt needs real locals and uses names whose buckets give the original's order.
 */
#include "../sdk/crt.h"

/* VC6 <math.h>, C++ on x86: the float functions are inline wrappers around the double ones. */
inline float __cdecl sinf(float _X)
{
    return ((float)sin((double)_X));
}
inline float __cdecl cosf(float _X)
{
    return ((float)cos((double)_X));
}

#define SDW_MEMBERS_Mat44 Mat44();

/* D3DVECTOR's inline members (<d3dtypes.h> with D3D_OVERLOADS, bodies from <d3dvec.inl>). */
#define SDW_MEMBERS_Vec3f                \
    Vec3f() {}                           \
    Vec3f(float _x, float _y, float _z)  \
    {                                    \
        x = _x;                          \
        y = _y;                          \
        z = _z;                          \
    }                                    \
    const float &operator[](int i) const \
    {                                    \
        return (&x)[i];                  \
    }                                    \
    float &operator[](int i)             \
    {                                    \
        return (&x)[i];                  \
    }                                    \
    Vec3f &operator/=(float s)           \
    {                                    \
        x /= s;                          \
        y /= s;                          \
        z /= s;                          \
        return *this;                    \
    }

#include "sdw_classes.h"

/* <d3dvec.inl>'s free operators, on Vec3f. */
inline Vec3f operator-(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vec3f operator*(float s, const Vec3f &v)
{
    return Vec3f(s * v.x, s * v.y, s * v.z);
}

inline float SquareMagnitude(const Vec3f &v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline float Magnitude(const Vec3f &v)
{
    return (float)sqrt(SquareMagnitude(v));
}

/* <d3dvec.inl>'s operator/ and Normalize, used by the tail below */
inline Vec3f operator/(const Vec3f &v, float s)
{
    return Vec3f(v.x / s, v.y / s, v.z / s);
}

inline Vec3f Normalize(const Vec3f &v)
{
    return v / Magnitude(v);
}

inline float DotProduct(const Vec3f &v1, const Vec3f &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3f CrossProduct(const Vec3f &v1, const Vec3f &v2)
{
    Vec3f result;
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return result;
}

/* The trig terms of the six Euler builders, at the original's offsets (-0x20 .. -0x4). */
struct EulerTerms {
    float sy, sx, cz, cxsy, cy, cx, sxsy, sz;
};

/* The quaternion terms of the two axis-angle builders (-0x38 .. -0x4); xx = 2*x*x and so on. */
struct QuatTerms {
    float s, zw, yz, xx, yw, zz, xz, yy, xy, xw, z, y, x, w;
};

/* 0x4077f0 Mat44_Ctor - the constructor: initialises nothing (its body is `return this`). It is Mat44's own: every
 * caller hands it a 0x40-byte matrix (new(0x40) x3 at 0x41a070, the temporaries of Mat44_Mul and its scalar twins,
 * static matrices), and the exe does not fold identical bodies (0x415c70 is a second, separate empty constructor;
 * Mat44_At and Mat44_AtConst are identical and both kept). */
Mat44::Mat44() {}

/* 0x4077fe - element-wise copy (operator=), returns *this. */
Mat44 &Mat44::Copy(const Mat44 &src)
{
    m[0][0] = src.m[0][0];
    m[0][1] = src.m[0][1];
    m[0][2] = src.m[0][2];
    m[0][3] = src.m[0][3];
    m[1][0] = src.m[1][0];
    m[1][1] = src.m[1][1];
    m[1][2] = src.m[1][2];
    m[1][3] = src.m[1][3];
    m[2][0] = src.m[2][0];
    m[2][1] = src.m[2][1];
    m[2][2] = src.m[2][2];
    m[2][3] = src.m[2][3];
    m[3][0] = src.m[3][0];
    m[3][1] = src.m[3][1];
    m[3][2] = src.m[3][2];
    m[3][3] = src.m[3][3];
    return *this;
}

/* 0x4078cc */
void Mat44::Zero()
{
    m[0][0] = m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][1] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][2] =
        m[2][3] = m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0.0f;
}

/* 0x407976 */
void Mat44::SetIdentity()
{
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] =
        m[3][2] = 0.0f;
}

/* 0x407a20 - rotation from Euler angles in radians, 3x3 = Rz*Ry*Rx; row and column 3 are the identity's. */
void Mat44::SetRotZYX(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz;
    m[0][1] = t.sxsy * t.cz + t.cx * t.sz;
    m[0][2] = -(t.cxsy * t.cz) + t.sx * t.sz;
    m[1][0] = -(t.cy * t.sz);
    m[1][1] = -(t.sxsy * t.sz) + t.cx * t.cz;
    m[1][2] = t.cxsy * t.sz + t.sx * t.cz;
    m[2][0] = t.sy;
    m[2][1] = -(t.sx * t.cy);
    m[2][2] = t.cx * t.cy;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x407ba1 - as SetRotZYX, 3x3 = Ry*Rz*Rx. */
void Mat44::SetRotYZX(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz;
    m[0][1] = t.cx * t.cy * t.sz + t.sxsy;
    m[0][2] = t.sx * t.cy * t.sz - t.cxsy;
    m[1][0] = -t.sz;
    m[1][1] = t.cx * t.cz;
    m[1][2] = t.sx * t.cz;
    m[2][0] = t.sy * t.cz;
    m[2][1] = t.cxsy * t.sz - t.sx * t.cy;
    m[2][2] = t.sxsy * t.sz + t.cx * t.cy;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x407d18 - as SetRotZYX, 3x3 = Rz*Rx*Ry. */
void Mat44::SetRotZXY(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz + t.sxsy * t.sz;
    m[0][1] = t.cx * t.sz;
    m[0][2] = t.sx * t.cy * t.sz - t.sy * t.cz;
    m[1][0] = t.sxsy * t.cz - t.cy * t.sz;
    m[1][1] = t.cx * t.cz;
    m[1][2] = t.sx * t.cy * t.cz + t.sy * t.sz;
    m[2][0] = t.cxsy;
    m[2][1] = -t.sx;
    m[2][2] = t.cx * t.cy;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x407e96 - as SetRotZYX, 3x3 = Rx*Rz*Ry. */
void Mat44::SetRotXZY(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz;
    m[0][1] = t.sz;
    m[0][2] = -(t.sy * t.cz);
    m[1][0] = t.sxsy - t.cx * t.cy * t.sz;
    m[1][1] = t.cx * t.cz;
    m[1][2] = t.sx * t.cy + t.cxsy * t.sz;
    m[2][0] = t.sx * t.cy * t.sz + t.cxsy;
    m[2][1] = -(t.sx * t.cz);
    m[2][2] = t.cx * t.cy - t.sxsy * t.sz;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x40800f - as SetRotZYX, 3x3 = Ry*Rx*Rz. */
void Mat44::SetRotYXZ(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz - t.sxsy * t.sz;
    m[0][1] = t.cy * t.sz + t.sxsy * t.cz;
    m[0][2] = -t.cx * t.sy;
    m[1][0] = -t.cx * t.sz;
    m[1][1] = t.cx * t.cz;
    m[1][2] = t.sx;
    m[2][0] = t.sy * t.cz + t.sx * t.cy * t.sz;
    m[2][1] = t.sy * t.sz - t.sx * t.cy * t.cz;
    m[2][2] = t.cx * t.cy;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x408192 - as SetRotZYX, 3x3 = Rx*Ry*Rz. */
void Mat44::SetRotXYZ(float x, float y, float z)
{
    EulerTerms t;
    t.cx = cosf(x);
    t.sx = sinf(x);
    t.cy = cosf(y);
    t.sy = sinf(y);
    t.cz = cosf(z);
    t.sz = sinf(z);
    t.cxsy = t.cx * t.sy;
    t.sxsy = t.sx * t.sy;
    m[0][0] = t.cy * t.cz;
    m[0][1] = t.cy * t.sz;
    m[0][2] = -t.sy;
    m[1][0] = t.sxsy * t.cz - t.cx * t.sz;
    m[1][1] = t.sxsy * t.sz + t.cx * t.cz;
    m[1][2] = t.sx * t.cy;
    m[2][0] = t.cxsy * t.cz + t.sx * t.sz;
    m[2][1] = t.cxsy * t.sz - t.sx * t.cz;
    m[2][2] = t.cx * t.cy;
    m[3][3] = 1.0f;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

/* 0x40830d - identity with translation (x, y, z). */
void Mat44::SetTranslation(float x, float y, float z)
{
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] = 0.0f;
}

/* 0x4083b6 - identity with translation v. */
void Mat44::SetTranslationV(const Vec3f &v)
{
    m[3][0] = v.x;
    m[3][1] = v.y;
    m[3][2] = v.z;
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] = 0.0f;
}

/* 0x408467 - diagonal (sx, sy, sz, 1). */
void Mat44::SetScale(float sx, float sy, float sz)
{
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
    m[3][3] = 1.0f;
    m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] =
        m[3][2] = 0.0f;
}

/* The locals of SetPerspective at the original's offsets (-0x10 .. -0x4). */
struct PerspTerms {
    float s, q, w, h;
};

/* 0x408510 - projection like the DX7 SDK's D3DUtil_SetProjectionMatrix, but with the aspect on _22 (the SDK puts it on
 * _11) and, where the SDK fails with E_INVALIDARG, a fallback of 10, 10, 1 when the clip range or sin(fov/2) is tiny. */
/* BYTES(slot-group): the terms are one PerspTerms struct only to pin the original frame offsets */
void Mat44::SetPerspective(float fov, float aspect, float zn, float zf)
{
    PerspTerms t;
    if (fabs(zf - zn) < 0.01f || fabs(sin(fov / 2)) < 0.01f) {
        t.w = 10.0f;
        t.h = 10.0f;
        t.q = 1.0f;
    } else {
        t.s = sinf(fov / 2);
        t.w = cosf(fov / 2) / t.s;
        t.h = t.w * aspect;
        t.q = zf / (zf - zn);
    }
    Zero();
    m[0][0] = t.w;
    m[1][1] = t.h;
    m[2][2] = t.q;
    m[2][3] = 1.0f;
    m[3][2] = -t.q * zn;
}

/* 0x40860c - view matrix, the DX7 SDK's D3DUtil_SetViewMatrix reworked: the identity when vFrom == vAt or vWorldUp is
 * parallel to the view direction (where the SDK retries two default up vectors and then fails). The SDK's names give the
 * original's slots except for vUp, whose bucket is too low; vNewUp is a name that fits (fLength -4, vRight -0x10,
 * vNewUp -0x1c, vView -0x28). A struct cannot pin them: an inline's argument that is a struct member is copied to a
 * temporary first, so vView /= fLength would gain a slot. */
/* BYTES(slot-name): vNewUp replaces the SDK's vUp, whose hash bucket is too low (fLength -4, vRight -0x10, vNewUp -0x1c, vView -0x28) */
void Mat44::SetLookAt(const Vec3f &vFrom, const Vec3f &vAt, const Vec3f &vWorldUp)
{
    Vec3f vView;
    float fLength;
    Vec3f vNewUp;
    Vec3f vRight;
    vView = vAt - vFrom;
    fLength = Magnitude(vView);
    if (fLength > 1e-6f) {
        vView /= fLength;
        vNewUp = vWorldUp - DotProduct(vWorldUp, vView) * vView;
        fLength = Magnitude(vNewUp);
        if (fLength > 1e-6f) {
            vNewUp /= fLength;
            vRight = CrossProduct(vNewUp, vView);
            m[0][0] = vRight.x;
            m[0][1] = vNewUp.x;
            m[0][2] = vView.x;
            m[0][3] = 0.0f;
            m[1][0] = vRight.y;
            m[1][1] = vNewUp.y;
            m[1][2] = vView.y;
            m[1][3] = 0.0f;
            m[2][0] = vRight.z;
            m[2][1] = vNewUp.z;
            m[2][2] = vView.z;
            m[2][3] = 0.0f;
            m[3][0] = -DotProduct(vFrom, vRight);
            m[3][1] = -DotProduct(vFrom, vNewUp);
            m[3][2] = -DotProduct(vFrom, vView);
            m[3][3] = 1.0f;
        } else
            SetIdentity();
    } else
        SetIdentity();
}

/* 0x4089c3 - rotation from the quaternion q = ((x, y, z) / sin(angle/2), cos(angle/2)): it DIVIDES by the sine where
 * SetRotAxisAngle multiplies, so it is only a rotation when (x, y, z) is already a scaled quaternion vector. */
void Mat44::SetRotAxisAngleDiv(float x, float y, float z, float angle)
{
    QuatTerms q;
    q.s = sin(angle / 2);
    q.w = cos(angle / 2);
    q.x = x / q.s;
    q.y = y / q.s;
    q.z = z / q.s;
    q.xx = 2 * q.x * q.x;
    q.yy = 2 * q.y * q.y;
    q.zz = 2 * q.z * q.z;
    q.xy = 2 * q.x * q.y;
    q.xz = 2 * q.x * q.z;
    q.xw = 2 * q.x * q.w;
    q.yz = 2 * q.y * q.z;
    q.yw = 2 * q.y * q.w;
    q.zw = 2 * q.z * q.w;
    m[0][0] = 1.0f - q.yy - q.zz;
    m[0][1] = q.xy - q.zw;
    m[0][2] = q.xz + q.yw;
    m[1][0] = q.xy + q.zw;
    m[1][1] = 1.0f - q.xx - q.zz;
    m[1][2] = q.yz - q.xw;
    m[2][0] = q.xz - q.yw;
    m[2][1] = q.yz + q.xw;
    m[2][2] = 1.0f - q.xx - q.yy;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

/* 0x408b6b - rotation by angle (radians) about axis: q = (axis * sin(angle/2), cos(angle/2)), axis assumed unit. */
void Mat44::SetRotAxisAngle(const Vec3f &axis, float angle)
{
    QuatTerms q;
    q.s = sin(angle / 2);
    q.w = cos(angle / 2);
    q.x = q.s * axis.x;
    q.y = q.s * axis.y;
    q.z = q.s * axis.z;
    q.xx = 2 * q.x * q.x;
    q.yy = 2 * q.y * q.y;
    q.zz = 2 * q.z * q.z;
    q.xy = 2 * q.x * q.y;
    q.xz = 2 * q.x * q.z;
    q.xw = 2 * q.x * q.w;
    q.yz = 2 * q.y * q.z;
    q.yw = 2 * q.y * q.w;
    q.zw = 2 * q.z * q.w;
    m[0][0] = 1.0f - q.yy - q.zz;
    m[0][1] = q.xy - q.zw;
    m[0][2] = q.xz + q.yw;
    m[1][0] = q.xy + q.zw;
    m[1][1] = 1.0f - q.xx - q.zz;
    m[1][2] = q.yz - q.xw;
    m[2][0] = q.xz - q.yw;
    m[2][1] = q.yz + q.xw;
    m[2][2] = 1.0f - q.xx - q.yy;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

/* 0x408d1b - &m[row][col] (D3DMATRIX's operator()). */
float &Mat44::At(int row, int col)
{
    return m[row][col];
}

/* 0x408d39 - the const twin of At, a separate (identical) function. */
const float &Mat44::AtConst(int row, int col) const
{
    return m[row][col];
}

/* 0x408d57 - every element *= s (operator*=(float)), returns *this. */
Mat44 &Mat44::ScaleInPlace(float s)
{
    m[0][0] *= s;
    m[0][1] *= s;
    m[0][2] *= s;
    m[0][3] *= s;
    m[1][0] *= s;
    m[1][1] *= s;
    m[1][2] *= s;
    m[1][3] *= s;
    m[2][0] *= s;
    m[2][1] *= s;
    m[2][2] *= s;
    m[2][3] *= s;
    m[3][0] *= s;
    m[3][1] *= s;
    m[3][2] *= s;
    m[3][3] *= s;
    return *this;
}

/* 0x408e55 - *this = *this * b (operator*=), row by row: each row is saved as (x, y, z) first, since it is overwritten
 * while it is read; its w is read in place, which is safe because m[i][3] is the last element written. */
Mat44 &Mat44::MulInPlace(const Mat44 &b)
{
    float x, y, z;
    x = m[0][0];
    y = m[0][1];
    z = m[0][2];
    m[0][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0] + m[0][3] * b.m[3][0];
    m[0][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1] + m[0][3] * b.m[3][1];
    m[0][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2] + m[0][3] * b.m[3][2];
    m[0][3] = x * b.m[0][3] + y * b.m[1][3] + z * b.m[2][3] + m[0][3] * b.m[3][3];
    x = m[1][0];
    y = m[1][1];
    z = m[1][2];
    m[1][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0] + m[1][3] * b.m[3][0];
    m[1][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1] + m[1][3] * b.m[3][1];
    m[1][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2] + m[1][3] * b.m[3][2];
    m[1][3] = x * b.m[0][3] + y * b.m[1][3] + z * b.m[2][3] + m[1][3] * b.m[3][3];
    x = m[2][0];
    y = m[2][1];
    z = m[2][2];
    m[2][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0] + m[2][3] * b.m[3][0];
    m[2][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1] + m[2][3] * b.m[3][1];
    m[2][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2] + m[2][3] * b.m[3][2];
    m[2][3] = x * b.m[0][3] + y * b.m[1][3] + z * b.m[2][3] + m[2][3] * b.m[3][3];
    x = m[3][0];
    y = m[3][1];
    z = m[3][2];
    m[3][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0] + m[3][3] * b.m[3][0];
    m[3][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1] + m[3][3] * b.m[3][1];
    m[3][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2] + m[3][3] * b.m[3][2];
    m[3][3] = x * b.m[0][3] + y * b.m[1][3] + z * b.m[2][3] + m[3][3] * b.m[3][3];
    return *this;
}

/* 0x4091fd - out = m * s (operator*(Mat44, float)). */
Mat44 *Mat44_MulScalar(Mat44 *out, const Mat44 *m, float s)
{
    Mat44 temp;
    temp.m[0][0] = s * m->m[0][0];
    temp.m[0][1] = s * m->m[0][1];
    temp.m[0][2] = s * m->m[0][2];
    temp.m[0][3] = s * m->m[0][3];
    temp.m[1][0] = s * m->m[1][0];
    temp.m[1][1] = s * m->m[1][1];
    temp.m[1][2] = s * m->m[1][2];
    temp.m[1][3] = s * m->m[1][3];
    temp.m[2][0] = s * m->m[2][0];
    temp.m[2][1] = s * m->m[2][1];
    temp.m[2][2] = s * m->m[2][2];
    temp.m[2][3] = s * m->m[2][3];
    temp.m[3][0] = s * m->m[3][0];
    temp.m[3][1] = s * m->m[3][1];
    temp.m[3][2] = s * m->m[3][2];
    temp.m[3][3] = s * m->m[3][3];
    out->Copy(temp);
    return out;
}

/* 0x4092dd - out = s * m (operator*(float, Mat44)). */
Mat44 *Mat44_ScalarMul(Mat44 *out, float s, const Mat44 *m)
{
    Mat44 temp;
    temp.m[0][0] = s * m->m[0][0];
    temp.m[0][1] = s * m->m[0][1];
    temp.m[0][2] = s * m->m[0][2];
    temp.m[0][3] = s * m->m[0][3];
    temp.m[1][0] = s * m->m[1][0];
    temp.m[1][1] = s * m->m[1][1];
    temp.m[1][2] = s * m->m[1][2];
    temp.m[1][3] = s * m->m[1][3];
    temp.m[2][0] = s * m->m[2][0];
    temp.m[2][1] = s * m->m[2][1];
    temp.m[2][2] = s * m->m[2][2];
    temp.m[2][3] = s * m->m[2][3];
    temp.m[3][0] = s * m->m[3][0];
    temp.m[3][1] = s * m->m[3][1];
    temp.m[3][2] = s * m->m[3][2];
    temp.m[3][3] = s * m->m[3][3];
    out->Copy(temp);
    return out;
}

/* 0x4093bd - out = a * b; safe when out aliases a or b. */
Mat44 *Mat44_Mul(Mat44 *out, const Mat44 *a, const Mat44 *b)
{
    Mat44 temp;
    temp.m[0][0] =
        a->m[0][0] * b->m[0][0] + a->m[0][1] * b->m[1][0] + a->m[0][2] * b->m[2][0] + a->m[0][3] * b->m[3][0];
    temp.m[0][1] =
        a->m[0][0] * b->m[0][1] + a->m[0][1] * b->m[1][1] + a->m[0][2] * b->m[2][1] + a->m[0][3] * b->m[3][1];
    temp.m[0][2] =
        a->m[0][0] * b->m[0][2] + a->m[0][1] * b->m[1][2] + a->m[0][2] * b->m[2][2] + a->m[0][3] * b->m[3][2];
    temp.m[0][3] =
        a->m[0][0] * b->m[0][3] + a->m[0][1] * b->m[1][3] + a->m[0][2] * b->m[2][3] + a->m[0][3] * b->m[3][3];
    temp.m[1][0] =
        a->m[1][0] * b->m[0][0] + a->m[1][1] * b->m[1][0] + a->m[1][2] * b->m[2][0] + a->m[1][3] * b->m[3][0];
    temp.m[1][1] =
        a->m[1][0] * b->m[0][1] + a->m[1][1] * b->m[1][1] + a->m[1][2] * b->m[2][1] + a->m[1][3] * b->m[3][1];
    temp.m[1][2] =
        a->m[1][0] * b->m[0][2] + a->m[1][1] * b->m[1][2] + a->m[1][2] * b->m[2][2] + a->m[1][3] * b->m[3][2];
    temp.m[1][3] =
        a->m[1][0] * b->m[0][3] + a->m[1][1] * b->m[1][3] + a->m[1][2] * b->m[2][3] + a->m[1][3] * b->m[3][3];
    temp.m[2][0] =
        a->m[2][0] * b->m[0][0] + a->m[2][1] * b->m[1][0] + a->m[2][2] * b->m[2][0] + a->m[2][3] * b->m[3][0];
    temp.m[2][1] =
        a->m[2][0] * b->m[0][1] + a->m[2][1] * b->m[1][1] + a->m[2][2] * b->m[2][1] + a->m[2][3] * b->m[3][1];
    temp.m[2][2] =
        a->m[2][0] * b->m[0][2] + a->m[2][1] * b->m[1][2] + a->m[2][2] * b->m[2][2] + a->m[2][3] * b->m[3][2];
    temp.m[2][3] =
        a->m[2][0] * b->m[0][3] + a->m[2][1] * b->m[1][3] + a->m[2][2] * b->m[2][3] + a->m[2][3] * b->m[3][3];
    temp.m[3][0] =
        a->m[3][0] * b->m[0][0] + a->m[3][1] * b->m[1][0] + a->m[3][2] * b->m[2][0] + a->m[3][3] * b->m[3][0];
    temp.m[3][1] =
        a->m[3][0] * b->m[0][1] + a->m[3][1] * b->m[1][1] + a->m[3][2] * b->m[2][1] + a->m[3][3] * b->m[3][1];
    temp.m[3][2] =
        a->m[3][0] * b->m[0][2] + a->m[3][1] * b->m[1][2] + a->m[3][2] * b->m[2][2] + a->m[3][3] * b->m[3][2];
    temp.m[3][3] =
        a->m[3][0] * b->m[0][3] + a->m[3][1] * b->m[1][3] + a->m[3][2] * b->m[2][3] + a->m[3][3] * b->m[3][3];
    out->Copy(temp);
    return out;
}

/* 0x409766 - transposes in place, swapping the six off-diagonal pairs. */
void Mat44::TransposeInPlace()
{
    float tmp;
    tmp = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = tmp;
    tmp = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = tmp;
    tmp = m[0][3];
    m[0][3] = m[3][0];
    m[3][0] = tmp;
    tmp = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = tmp;
    tmp = m[1][3];
    m[1][3] = m[3][1];
    m[3][1] = tmp;
    tmp = m[2][3];
    m[2][3] = m[3][2];
    m[3][2] = tmp;
}

/* ======================================================================== Mat44, the tail (0x409827-0x409e87) */

/* 0x409827 - out = the transpose of this (all 16 elements). */
void Mat44::Transpose(Mat44 *out)
{
    out->m[0][0] = m[0][0];
    out->m[1][1] = m[1][1];
    out->m[2][2] = m[2][2];
    out->m[3][3] = m[3][3];
    out->m[0][1] = m[1][0];
    out->m[0][2] = m[2][0];
    out->m[0][3] = m[3][0];
    out->m[1][0] = m[0][1];
    out->m[2][0] = m[0][2];
    out->m[3][0] = m[0][3];
    out->m[1][2] = m[2][1];
    out->m[1][3] = m[3][1];
    out->m[2][1] = m[1][2];
    out->m[3][1] = m[1][3];
    out->m[2][3] = m[3][2];
    out->m[3][2] = m[2][3];
}

/* 0x4098f2 - transposes the upper-left 3x3 (the rotation) in place; row and column 3 are left alone. */
void Mat44::Transpose3x3InPlace()
{
    float tmp;
    tmp = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = tmp;
    tmp = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = tmp;
    tmp = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = tmp;
}

/* 0x409959 - out = this with its upper-left 3x3 transposed; row and column 3 are copied as they are. */
void Mat44::Transpose3x3(Mat44 *out)
{
    out->m[0][0] = m[0][0];
    out->m[1][1] = m[1][1];
    out->m[2][2] = m[2][2];
    out->m[3][3] = m[3][3];
    out->m[0][1] = m[1][0];
    out->m[0][2] = m[2][0];
    out->m[1][0] = m[0][1];
    out->m[2][0] = m[0][2];
    out->m[1][2] = m[2][1];
    out->m[2][1] = m[1][2];
    out->m[0][3] = m[0][3];
    out->m[3][0] = m[3][0];
    out->m[1][3] = m[1][3];
    out->m[3][1] = m[3][1];
    out->m[2][3] = m[2][3];
    out->m[3][2] = m[3][2];
}

/* 0x409a24 - normalises the three rotation columns in place (no zero-length guard). */
void Mat44::NormalizeColumnsInPlace()
{
    Vec3f v, n;
    v.x = m[0][0];
    v.y = m[1][0];
    v.z = m[2][0];
    n = Normalize(v);
    m[0][0] = n.x;
    m[1][0] = n.y;
    m[2][0] = n.z;
    v.x = m[0][1];
    v.y = m[1][1];
    v.z = m[2][1];
    n = Normalize(v);
    m[0][1] = n.x;
    m[1][1] = n.y;
    m[2][1] = n.z;
    v.x = m[0][2];
    v.y = m[1][2];
    v.z = m[2][2];
    n = Normalize(v);
    m[0][2] = n.x;
    m[1][2] = n.y;
    m[2][2] = n.z;
}

/* 0x409c03 - the same into out: only out's three rotation columns are written. */
void Mat44::NormalizeColumns(Mat44 *out)
{
    Vec3f v, n;
    v.x = m[0][0];
    v.y = m[1][0];
    v.z = m[2][0];
    n = Normalize(v);
    out->m[0][0] = n.x;
    out->m[1][0] = n.y;
    out->m[2][0] = n.z;
    v.x = m[0][1];
    v.y = m[1][1];
    v.z = m[2][1];
    n = Normalize(v);
    out->m[0][1] = n.x;
    out->m[1][1] = n.y;
    out->m[2][1] = n.z;
    v.x = m[0][2];
    v.y = m[1][2];
    v.z = m[2][2];
    n = Normalize(v);
    out->m[0][2] = n.x;
    out->m[1][2] = n.y;
    out->m[2][2] = n.z;
}

/* 0x409de4 - dst = src * this as a point (row vector, translation row added). */
void Mat44::TransformPoint(const Vec3f *src, Vec3f *dst)
{
    dst->x = src->x * m[0][0] + src->y * m[1][0] + src->z * m[2][0] + m[3][0];
    dst->y = src->x * m[0][1] + src->y * m[1][1] + src->z * m[2][1] + m[3][1];
    dst->z = src->x * m[0][2] + src->y * m[1][2] + src->z * m[2][2] + m[3][2];
}
