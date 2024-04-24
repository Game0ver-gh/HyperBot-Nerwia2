#pragma once

class Screen
{
public:
	Screen() : m_dwVS(0) {}
	virtual ~Screen() { __DestroyPDTVertexBufferList(); }
	
	void RenderLine3D(const Vector& from, const Vector& to, const Color& color);
	void RenderCircle3D(const Vector& center, float radius, int step, const Color& color, bool check_collision = true, bool show_grid = false);
	void RenderAABB3D(const Vector& min, const Vector& max, const Color& color);
	void RenderCircleProgress3D(const Vector& center, float radius, int step, float value, float max_value, const Color& color, bool check_collision = true);

	
	//If true - drawnings will collide with other textures
	void SetDepth(bool enable);
	void RenderDrawList();
	void StartRender();
	void EndRender();
	void UpdateWorldState();

	void DrawStringDbg(const Vector2D& min, bool center, const char* msg, ...);
	void DrawString(const Vector2D& min, DWORD EStrFlag, const Color& color, const char* msg, ...);
	void DrawRectFilled(const Vector2D& vLT, const Vector2D& vRB, const Color& col);
	void DrawRectFilled(const Rect& bb, const Color& col);
	void DrawRectGradient(const Vector2D& min, const Vector2D& max, const Color& leftTop, const Color& rightTop, const Color& leftBot, const Color& rightBot);
	void DrawRectGradient(const Rect& bb, const Color& leftTop, const Color& rightTop, const Color& leftBot, const Color& rightBot);
	void DrawRoundedRectFilled(const Vector2D& min, const Vector2D& max, float rnd, const Color& col);
	void DrawRoundedRectFilled(const Rect& bb, float rnd, const Color& col);
	void DrawRoundedRect(const Vector2D& min, const Vector2D& max, float rnd, const Color& col);
	void DrawRoundedRect(const Rect& bb, float rnd, const Color& col);
	void DrawRect(const Vector2D& min, const Vector2D& max, const Color& col);
	void DrawRect(const Rect& bb, const Color& col);
	void DrawRectOutlined(const Vector2D& min, const Vector2D& max, const Color& col, Color outline = Color::Black());
	void DrawRectOutlined(const Rect& bb, const Color& col, Color outline = Color::Black());
	void DrawCircle(const Vector2D& pos, float rad, float step, const Color& color);
	void DrawCircleFilled(const Vector2D& pos, float rad, float step, const Color& Color);
	void DrawCircleOutlined(const Vector2D& pos, int rad, int seg, const Color& col);
	void DrawCircleProgressBar(const Vector2D& pos, int rad, int seg, float value, float maxValue, const Color& color, float thickness = 1.0f, bool outline = false, Vector2D* lastPointsOut = nullptr);
	void DrawLine(const Vector2D& pos1, const Vector2D& pos2, const Color& col);
	void DrawLine(const Rect& bb, const Color& col);
	void DrawLine(const std::vector<Vector2D>& points, const Color& col);
	void DrawLineGlow(const Vector2D& min, const Vector2D& max, const Color& color, const Color& glowColor, const size_t glowLen = 5);
	void DrawTriangleFilled(const std::array<Vector2D, 3>& points, const Color& color);
	void DrawTriangle(const std::array<Vector2D, 3>& points, const Color& color);
	void DrawCountingBar(const Vector2D& center, const Vector2D& size, float value, float maxValue, bool forceShow = false, const Color& color = Color(0, 128, 255, 144));
	void DrawProgressBar(const Vector2D& center, const Vector2D& size, float value, float maxValue, const Color& color = Color(0, 128, 255, 144));
	void DrawImageRotated(ImTextureID tex_id, const Vector2D& center, const Vector2D& size, float angle, const Color& color);
	void DrawImage(ImTextureID tex_id, const Vector2D& min, const Vector2D& size, const Color& color = Color::White());
	
	static Vector2D GetTextSize(const char* msg, ...);
	static Vector2D GetCursorPos();

private:
	void BackupRenderState();
	void SetRenderState();
	void RestoreRenderState();

	void _RenderLine(const Vector& from, const Vector& to, D3DCOLOR color);
	void _RenderCircle(const Vector& center, float radius, int step, D3DCOLOR color, bool check_collision = true, bool show_grid = false);
	void _RenderAABB(const Vector& min, const Vector& max, D3DCOLOR color);
	void _RenderCircleProgress3D(const Vector& center, float radius, int step, float value, float max_value, D3DCOLOR color, bool check_collision = true);

	struct SD3DVertex 
	{
		float x, y, z, rhw;
		DWORD colour;
	};

	struct SPDTVertexRaw
	{
		float px, py, pz;
		DWORD diffuse;
		float u, v;
	};

	typedef struct SPDTVertex
	{
		D3DVECTOR	position;
		DWORD	diffuse;
		Vector2D texCoord;
	} TPDTVertex;

	class CStreamData
	{
	public:
		CStreamData(LPDIRECT3DVERTEXBUFFER8 pStreamData = NULL, UINT Stride = 0) : m_lpStreamData(pStreamData), m_Stride(Stride)
		{
		}

		bool operator == (const CStreamData& rhs) const
		{
			return ((m_lpStreamData == rhs.m_lpStreamData) && (m_Stride == rhs.m_Stride));
		}

		LPDIRECT3DVERTEXBUFFER8	m_lpStreamData;
		UINT					m_Stride;
	};

	bool _SetPDTStream(SPDTVertexRaw* pSrcVertices, UINT uVtxCount);
	void _SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);
	bool __CreatePDTVertexBufferList();
	void __DestroyPDTVertexBufferList();
	void _SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture);
	void _SetVertexShader(DWORD dwShader);
	
	struct RenderCall
	{
		int fn_call_id = -1, r = -1, step = -1;
		Vector p1{}, p2{};
		D3DCOLOR color_rgba = 0x0;
		bool z_enable = true, check_collision = true, grid = false;
		float min, max;
	};

	DWORD m_dwVS;
	D3DMATRIX world_state{};
	bool m_z_enable = true;
	std::vector<RenderCall> m_render_calls;
	CStreamData				m_StreamData[16];
};

enum class EStrFlag
{
	NONE = 0,
	CENTER_X = 1 << 1,
	RIGHT = 1 << 2,
	TOP = 1 << 3,
	CENTER_Y = 1 << 4,
};

inline DWORD operator|(DWORD& a, EStrFlag b)
{
	return a | static_cast<DWORD>(b);
}

inline DWORD& operator|=(DWORD& a, EStrFlag b)
{
	return a |= static_cast<DWORD>(b);
}

inline DWORD operator&(DWORD& a, EStrFlag b)
{
	return a & static_cast<DWORD>(b);
}

inline DWORD& operator&=(DWORD& a, EStrFlag b)
{
	return a &= static_cast<DWORD>(b);
}
