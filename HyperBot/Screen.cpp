#include "pch.h"
#include "Screen.h"
#define SetVertex SetVertexShader
#define GetVertex GetVertexShader
#define IndexBuffer LPDIRECT3DINDEXBUFFER8
#define M_PI 3.14159265358979323846
#define draw_list ImGui::GetBackgroundDrawList()
#define PDT_VERTEXBUFFER_NUM 100
#define DEFAULT_IB_NUM 7
#define PDT_VERTEX_NUM 16
LPDIRECT3DVERTEXBUFFER8	ms_alpd3dPDTVB[PDT_VERTEXBUFFER_NUM];
LPDIRECT3DINDEXBUFFER8	ms_alpd3dDefIB[DEFAULT_IB_NUM];
LPDIRECT3DBASETEXTURE8	m_Textures[16];
DWORD					m_dwVertexShader;
size_t					m_line_count = 0;

void Screen::RenderLine3D(const Vector& from, const Vector& to, const Color& color)
{
	RenderCall rc;
	rc.fn_call_id = 0;
	rc.p1 = from;
	rc.p2 = to;
	rc.color_rgba = D3DCOLOR_RGBA(color.r(), color.g(), color.b(), color.a());
	rc.z_enable = m_z_enable;
	m_render_calls.push_back(rc);
}

void Screen::RenderCircle3D(const Vector& center, float radius, int step, const Color& color, bool check_collision, bool show_grid)
{
	RenderCall rc;
	rc.fn_call_id = 1;
	rc.p1 = center;
	rc.color_rgba = D3DCOLOR_RGBA(color.r(), color.g(), color.b(), color.a());;
	rc.z_enable = m_z_enable;
	rc.check_collision = check_collision;
	rc.step = step;
	rc.r = radius;
	rc.grid = show_grid;
	m_render_calls.push_back(rc);
}

void Screen::RenderAABB3D(const Vector& min, const Vector& max, const Color& color)
{
	RenderCall rc;
	rc.fn_call_id = 2;
	rc.p1 = min;
	rc.p2 = max;
	rc.color_rgba = D3DCOLOR_RGBA(color.r(), color.g(), color.b(), color.a());;
	rc.z_enable = m_z_enable;
	m_render_calls.push_back(rc);
}

void Screen::RenderCircleProgress3D(const Vector& center, float radius, int step, float value, float max_value, const Color& color, bool check_collision)
{
	RenderCall rc;
	rc.fn_call_id = 3;
	rc.p1 = center;
	rc.color_rgba = D3DCOLOR_RGBA(color.r(), color.g(), color.b(), color.a());;
	rc.z_enable = m_z_enable;
	rc.check_collision = check_collision;
	rc.step = step;
	rc.r = radius;
	rc.min = value;
	rc.max = max_value;
	m_render_calls.push_back(rc);
}

void Screen::_RenderCircleProgress3D(const Vector& center, float radius, int step, float value, float max_value, D3DCOLOR color, bool check_collision)
{
	if (step <= 0) return;
	std::vector <Vector> points;
	points.reserve(step);
	float stepSize = 2.f * M_PI / step;

	for (int i = 0; i < step; i++)
	{
		float angle = i * stepSize;
		Vector point;
		point.x = center.x + radius * std::cos(angle);
		point.y = center.y + radius * std::sin(angle);
		point.z = (check_collision ? (G::game->GetBackground()->GetTerrainHeight(point) + 10.f) : center.z);
		points.push_back(point);
	}

	int render_step = value * step / max_value;
	for (int i = 0; i < step; i++)
	{
		if (i > render_step) break;
		_RenderLine(points[i], points[(i + 1) % step], color);
	}
}

void Screen::SetDepth(bool enable)
{
	m_z_enable = enable;
}

void Screen::RenderDrawList()
{
	if (!m_render_calls.empty())
	{
		for (const auto& rc : m_render_calls)
		{
			static DWORD prev_z_value = 0;
			if (static_cast<DWORD>(rc.z_enable) != prev_z_value)
			{
				prev_z_value = rc.z_enable;
				DX8::device->SetRenderState(D3DRS_ZENABLE, prev_z_value);
			}
			
			switch (rc.fn_call_id)
			{
			case 0:
				_RenderLine(rc.p1, rc.p2, rc.color_rgba);
				break;
			case 1:
				_RenderCircle(rc.p1, rc.r, rc.step, rc.color_rgba, rc.check_collision, rc.grid);
				break;
			case 2:
				_RenderAABB(rc.p1, rc.p2, rc.color_rgba);
				break;
			case 3:
				_RenderCircleProgress3D(rc.p1, rc.r, rc.step, rc.min, rc.max, rc.color_rgba, rc.check_collision);
				break;
			}
		}

		m_render_calls.clear();
	}
}

void Screen::StartRender()
{
	m_line_count = 0;
	static bool init = true;
	if (init)
	{
		init = false;
		__CreatePDTVertexBufferList();
	}
	for (int i = 0; i < 16; i++)
		m_StreamData[i] = CStreamData();
	BackupRenderState();
	SetRenderState();
}

void Screen::EndRender()
{
	RestoreRenderState();
}

void Screen::UpdateWorldState()
{
	DX8::device->GetTransform(D3DTS_WORLD, &world_state);
}

DWORD tex_state_backup[2][15];
DWORD state_backup[15];
D3DMATRIX stateOldGame;

void Screen::BackupRenderState()
{
	DX8::device->GetVertexShader(&m_dwVS);
	DX8::device->GetRenderState(D3DRS_CULLMODE, &state_backup[0]);
	DX8::device->GetRenderState(D3DRS_TEXTUREFACTOR, &state_backup[1]);
	DX8::device->GetRenderState(D3DRS_LIGHTING, &state_backup[2]);
	DX8::device->GetRenderState(D3DRS_ZENABLE, &state_backup[3]);

	DX8::device->GetTextureStageState(0, D3DTSS_COLORARG1, &tex_state_backup[0][0]);
	DX8::device->GetTextureStageState(0, D3DTSS_COLOROP, &tex_state_backup[0][1]);
	DX8::device->GetTextureStageState(0, D3DTSS_ALPHAOP, &tex_state_backup[0][2]);
	DX8::device->GetTextureStageState(1, D3DTSS_COLOROP, &tex_state_backup[1][3]);
	DX8::device->GetTextureStageState(1, D3DTSS_ALPHAOP, &tex_state_backup[1][4]);
	
	DX8::device->GetTransform(D3DTS_WORLD, &stateOldGame);
}

void Screen::SetRenderState()
{
	DX8::device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	DX8::device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	DX8::device->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	DX8::device->SetTexture(0, NULL);
	DX8::device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	DX8::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	DX8::device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	DX8::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	DX8::device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	
	DX8::device->SetTransform(D3DTS_WORLD, &world_state);
}

void Screen::RestoreRenderState()
{
	DX8::device->SetVertexShader(m_dwVS);

	DX8::device->SetRenderState(D3DRS_CULLMODE, state_backup[0]);
	DX8::device->SetRenderState(D3DRS_TEXTUREFACTOR, state_backup[1]);
	DX8::device->SetRenderState(D3DRS_LIGHTING, state_backup[2]);
	DX8::device->SetRenderState(D3DRS_ZENABLE, state_backup[3]);
	
	DX8::device->SetTextureStageState(0, D3DTSS_COLORARG1, tex_state_backup[0][0]);
	DX8::device->SetTextureStageState(0, D3DTSS_COLOROP, tex_state_backup[0][1]);
	DX8::device->SetTextureStageState(0, D3DTSS_ALPHAOP, tex_state_backup[0][2]);
	DX8::device->SetTextureStageState(1, D3DTSS_COLOROP, tex_state_backup[1][3]);
	DX8::device->SetTextureStageState(1, D3DTSS_ALPHAOP, tex_state_backup[1][4]);
	
	DX8::device->SetTransform(D3DTS_WORLD, &stateOldGame);
}

void Screen::_RenderLine(const Vector& from, const Vector& to, D3DCOLOR color)
{
	SPDTVertexRaw pVertex[2] =
	{
		{ from.x, -from.y, from.z, color, 0.0f, 0.0f },
		{ to.x, -to.y, to.z, color, 0.0f, 0.0f }
	};

	if (_SetPDTStream(pVertex, 2))
	{
		DX8::device->SetTexture(0, nullptr);
		DX8::device->SetTexture(1, nullptr);
		DX8::device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		DX8::device->DrawPrimitive(D3DPT_LINELIST, 0, 1);
	}
}

void Screen::_RenderCircle(const Vector& center, float radius, int step, D3DCOLOR color, bool check_collision, bool show_grid)
{
	if (step < 2) return;
	step = std::round(step / 2) * 2;
	std::vector <Vector> points;
	points.reserve(step);
	float stepSize = 2.f * M_PI / step;

	auto* bg= G::game->GetBackground();

	for (int i = 0; i < step; i++)
	{
		float angle = i * stepSize;
		Vector point;
		point.x = center.x + radius * std::cos(angle);
		point.y = center.y + radius * std::sin(angle);
		point.z = (check_collision ? (bg->GetTerrainHeight(point) + 10.f) : center.z);
		points.push_back(point);
	}

	for (int i = 0; i < step; i++)
		_RenderLine(points[i], points[(i + 1) % step], color);

	if (show_grid)
	{
		int line_step = step / 2;
		for (int i = 1; i < line_step; i++)
		{
			
			if (check_collision)
			{
				//horizontal lines
				std::vector<Vector> delta_points;
				if (Math::CalcDeltaPoints(
					points[step - i], points[i],
					points[step - i].Distance(points[i]) / line_step,
					delta_points))
				{
					for (size_t i = 0; i < delta_points.size() - 1; i++)
					{
						delta_points[i].z = bg->GetTerrainHeight(delta_points[i]) + 10.f;
						delta_points[i + 1].z = bg->GetTerrainHeight(delta_points[i + 1]) + 10.f;
						_RenderLine(delta_points[i], delta_points[i + 1], color);
					}
				}

				delta_points.clear();

				//vertical lines
				if (Math::CalcDeltaPoints(
					points[i - 1], points[step - i - 2],
					points[i - 1].Distance(points[step - i - 2]) / line_step,
					delta_points))
				{
					for (size_t i = 0; i < delta_points.size() - 1; i++)
					{
						delta_points[i].z = bg->GetTerrainHeight(delta_points[i]) + 10.f;
						delta_points[i + 1].z = bg->GetTerrainHeight(delta_points[i + 1]) + 10.f;
						_RenderLine(delta_points[i], delta_points[i + 1], color);
					}
				}
			}
			else
			{
				_RenderLine(points[step - i], points[i], color);
				_RenderLine(points[i - 1], points[step - i - 2], color);
			}
		}
	}

}

void Screen::_RenderAABB(const Vector& min, const Vector& max, D3DCOLOR color)
{
	Vector v1(min.x, min.y, min.z);
	Vector v2(max.x, min.y, min.z);
	Vector v3(max.x, max.y, min.z);
	Vector v4(min.x, max.y, min.z);
	Vector v5(min.x, min.y, max.z);
	Vector v6(max.x, min.y, max.z);
	Vector v7(max.x, max.y, max.z);
	Vector v8(min.x, max.y, max.z);
	_RenderLine(v1, v2, color);
	_RenderLine(v2, v3, color);
	_RenderLine(v3, v4, color);
	_RenderLine(v4, v1, color);

	_RenderLine(v5, v6, color);
	_RenderLine(v6, v7, color);
	_RenderLine(v7, v8, color);
	_RenderLine(v8, v5, color);

	_RenderLine(v1, v5, color);
	_RenderLine(v2, v6, color);
	_RenderLine(v3, v7, color);
	_RenderLine(v4, v8, color);
}

bool Screen::_SetPDTStream(SPDTVertexRaw* pSrcVertices, UINT uVtxCount)
{
	if (!uVtxCount)
		return false;

	static DWORD s_dwVBPos = 0;

	if (s_dwVBPos >= 100)
		s_dwVBPos = 0;

	IDirect3DVertexBuffer8* plpd3dFillRectVB = ms_alpd3dPDTVB[s_dwVBPos];
	++s_dwVBPos;

	assert(PDT_VERTEX_NUM >= uVtxCount);
	if (uVtxCount >= PDT_VERTEX_NUM)
		return false;

	TPDTVertex* pDstVertices=nullptr;
	if (FAILED(
		plpd3dFillRectVB->Lock(0, sizeof(TPDTVertex) * uVtxCount, (BYTE**)&pDstVertices, D3DLOCK_DISCARD)
	))
	{
		_SetStreamSource(0, NULL, 0);
		return false;
	}


	memcpy(pDstVertices, pSrcVertices, sizeof(TPDTVertex) * uVtxCount);

	plpd3dFillRectVB->Unlock();

	_SetStreamSource(0, plpd3dFillRectVB, sizeof(TPDTVertex));

	return true;
}

void Screen::_SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride)
{
	CStreamData kStreamData(pStreamData, Stride);
	if (m_StreamData[StreamNumber] == kStreamData)
		return;

	DX8::device->SetStreamSource(StreamNumber, pStreamData, Stride);
	m_StreamData[StreamNumber] = kStreamData;
}

bool Screen::__CreatePDTVertexBufferList()
{
	for (UINT i = 0; i < PDT_VERTEXBUFFER_NUM; ++i)
	{
		if (FAILED(
			DX8::device->CreateVertexBuffer(
				sizeof(TPDTVertex) * PDT_VERTEX_NUM,
				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
				D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
				D3DPOOL_SYSTEMMEM,
				&ms_alpd3dPDTVB[i])
		))
			return false;
	}
	return true;
}

void Screen::__DestroyPDTVertexBufferList()
{
	for (UINT i = 0; i < PDT_VERTEXBUFFER_NUM; ++i)
	{
		if (ms_alpd3dPDTVB[i])
		{
			ms_alpd3dPDTVB[i]->Release();
			ms_alpd3dPDTVB[i] = NULL;
		}
	}
}

void Screen::_SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture)
{
	if (pTexture == m_Textures[dwStage])
		return;

	DX8::device->SetTexture(dwStage, pTexture);
	m_Textures[dwStage] = pTexture;
}

void Screen::_SetVertexShader(DWORD dwShader)
{
	if (m_dwVertexShader == dwShader)
		return;

	DX8::device->SetVertexShader(dwShader);
	m_dwVertexShader = dwShader;
}



// =============================================================================================================




void Screen::DrawStringDbg(const Vector2D& min, bool center, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024]{};
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	
	ImVec2 vSize = ImGui::CalcTextSize(buf);
	ImVec2 vMin = { min.x, min.y };

	if (center)
	{
		vMin.x -= vSize.x * 0.5f;
		vMin.y -= vSize.y * 0.5f;
	}

	draw_list->AddText(vMin, Color::White(), buf);
}

void Screen::DrawString(const Vector2D& min, DWORD EStrFlag, const Color& color, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);

	if (!buf) return;

	ImVec2 vSize = ImGui::CalcTextSize(buf);
	ImVec2 vMin = { min.x, min.y };

	//Position adjusting
	if (EStrFlag & EStrFlag::CENTER_X)
		vMin.x -= vSize.x * 0.5f;
	if (EStrFlag & EStrFlag::CENTER_Y)
		vMin.y -= vSize.y * 0.5f;
	if (EStrFlag & EStrFlag::TOP)
		vMin.y -= vSize.y;
	if (EStrFlag & EStrFlag::RIGHT)
		vMin.x -= vSize.x;

	draw_list->AddText(ImVec2(vMin.x + 1, vMin.y), Color::Black(color.a()), buf);
	draw_list->AddText(ImVec2(vMin.x - 1, vMin.y), Color::Black(color.a()), buf);
	draw_list->AddText(ImVec2(vMin.x, vMin.y + 1), Color::Black(color.a()), buf);
	draw_list->AddText(ImVec2(vMin.x, vMin.y - 1), Color::Black(color.a()), buf);

	draw_list->AddText(vMin, color, buf);
}

void Screen::DrawRectFilled(const Vector2D& vLT, const Vector2D& vRB, const Color& col)
{
	draw_list->AddRectFilled({ vLT.x, vLT.y }, { vRB.x, vRB.y }, col);
}

void Screen::DrawRectFilled(const Rect& bb, const Color& col)
{
	DrawRectFilled(bb.GetMin(), bb.GetMax(), col);
}

void Screen::DrawRectGradient(const Vector2D& min, const Vector2D& max, const Color& leftTop, const Color& rightTop, const Color& leftBot, const Color& rightBot)
{
	draw_list->AddRectFilledMultiColor({ min.x, min.y }, { max.x, max.y }, leftTop, rightTop, rightBot, leftBot);
}

void Screen::DrawRectGradient(const Rect& bb, const Color& leftTop, const Color& rightTop, const Color& leftBot, const Color& rightBot)
{
	DrawRectGradient(bb.GetMin(), bb.GetMax(), leftTop, rightTop, leftBot, rightBot);
}

void Screen::DrawRoundedRectFilled(const Vector2D& min, const Vector2D& max, float rnd, const Color& col)
{
	draw_list->AddRectFilled({ min.x, min.y }, { max.x, max.y }, col, rnd);
}

void Screen::DrawRoundedRectFilled(const Rect& bb, float rnd, const Color& col)
{
	DrawRoundedRectFilled(bb.GetMin(), bb.GetMax(), rnd, col);
}

void Screen::DrawRoundedRect(const Vector2D& min, const Vector2D& max, float rnd, const Color& col)
{
	draw_list->AddRect({ min.x, min.y }, { max.x, max.y }, (col), rnd);
}

void Screen::DrawRoundedRect(const Rect& bb, float rnd, const Color& col)
{
	DrawRoundedRect(bb.GetMin(), bb.GetMax(), rnd, col);
}

void Screen::DrawRect(const Vector2D& min, const Vector2D& max, const Color& col)
{
	draw_list->AddRect({ min.x, min.y }, { max.x, max.y }, (col));
}

void Screen::DrawRect(const Rect& bb, const Color& col)
{
	DrawRect(bb.GetMin(), bb.GetMax(), col);
}

void Screen::DrawRectOutlined(const Vector2D& min, const Vector2D& max, const Color& col, Color outline)
{
	DrawRect(Vector2D(min.x - 1, min.y - 1), Vector2D(max.x + 1, max.y + 1), outline(col.a()));
	DrawRect(Vector2D(min.x + 1, min.y + 1), Vector2D(max.x - 1, max.y - 1), outline(col.a()));
	DrawRect(min, max, col);
}

void Screen::DrawRectOutlined(const Rect& bb, const Color& col, Color outline)
{
	DrawRectOutlined(bb.GetMin(), bb.GetMax(), col, outline);
}

void Screen::DrawCircleOutlined(const Vector2D& pos, int rad, int seg, const Color& col)
{
	draw_list->AddCircle({ pos.x, pos.y }, rad - 1, Color::Black(col.a()));
	draw_list->AddCircle({ pos.x, pos.y }, rad + 1, Color::Black(col.a()));
	draw_list->AddCircle({ pos.x, pos.y }, rad, (col));
}

void Screen::DrawCircleProgressBar(const Vector2D& pos, int rad, int seg, float value, float maxValue, const Color& color, float thickness, bool outline, Vector2D* lastPointsOut)
{
	if (outline)
		draw_list->AddCircleProgress({ pos.x, pos.y }, rad, seg, value, maxValue, Color::Black(color.a()), thickness + 2.0f);
	draw_list->AddCircleProgress({ pos.x, pos.y }, rad, seg, value, maxValue, color, thickness, (ImVec2*)lastPointsOut);
}

void Screen::DrawLine(const Vector2D& pos1, const Vector2D& pos2, const Color& col)
{
	draw_list->AddLine({ pos1.x, pos1.y }, { pos2.x, pos2.y }, col);
}

void Screen::DrawLine(const Rect& bb, const Color& col)
{
	DrawLine(bb.GetMin(), bb.GetMax(), col);
}

void Screen::DrawLine(const std::vector<Vector2D>& points, const Color& col)
{
	if (points.size() < 2) return;

	draw_list->AddPolyline((ImVec2*)points.data(), points.size(), col, false, 1.f);
}

void Screen::DrawTriangleFilled(const std::array<Vector2D, 3>& points, const Color& color)
{
	draw_list->AddTriangleFilled({ points[0].x, points[0].y }, { points[1].x, points[1].y }, { points[2].x, points[2].y }, color);
}

void Screen::DrawTriangle(const std::array<Vector2D, 3>& points, const Color& color)
{
	draw_list->AddTriangle({ points[0].x, points[0].y }, { points[1].x, points[1].y }, { points[2].x, points[2].y }, color);
}

void Screen::DrawCountingBar(const Vector2D& center, const Vector2D& size, float value, float maxValue, bool forceShow, const Color& color)
{
	if (maxValue <= 0 || maxValue <= value) return;
	float lenght = (value * size.x) / maxValue;

	if (lenght > 2 || forceShow)
	{
		//BG
		draw_list->AddRectFilled(
			ImVec2(center.x - size.x / 2, center.y),
			ImVec2(center.x + size.x / 2, center.y + size.y),
			Color::Black(color.a()), 2);

		//Bar
		draw_list->AddRectFilled(
			ImVec2((center.x - size.x / 2) + 2, (center.y) + 2),
			ImVec2(((center.x - size.x / 2) + 2) + (lenght - 2), (center.y + size.y) - 2),
			color, 2);
	}
}

void Screen::DrawProgressBar(const Vector2D& center, const Vector2D& size, float value, float maxValue, const Color& color)
{
	if (maxValue <= 0 || maxValue <= value) return;

	auto lenght = (value * size.x) / maxValue - 2.f;
	auto bb = Rect(Vector2D(center.x - (size.x / 2), center.y), Vector2D(center.x + (size.x / 2), center.y + size.y));
	lenght = std::clamp(lenght, 0.0f, bb.GetSize().x);

	//BG
	DrawRectFilled(bb.GetMin(), bb.GetMax(), Color::Black(190));

	//Bar
	DrawRectFilled(
		Vector2D(bb.GetMin().x + 2, center.y + 2),
		Vector2D(bb.GetMin().x + (lenght + 2), bb.GetMax().y - 2),
		color);
}

void Screen::DrawImageRotated(ImTextureID tex_id, const Vector2D& center, const Vector2D& size, float angle, const Color& color)
{
	auto ImRotate = [](const Vector2D& v, float cos_a, float sin_a)
	{
		return Vector2D(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
	};

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	Vector2D pos[4] =
	{
		center + ImRotate(Vector2D(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(Vector2D(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(Vector2D(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(Vector2D(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
	};
	Vector2D uvs[4] =
	{
		Vector2D(0.0f, 0.0f),
		Vector2D(1.0f, 0.0f),
		Vector2D(1.0f, 1.0f),
		Vector2D(0.0f, 1.0f)
	};

	draw_list->AddImageQuad(tex_id,
		{ pos[0].x, pos[0].y }, { pos[1].x, pos[1].y }, { pos[2].x, pos[2].y }, { pos[3].x, pos[3].y },
		{ pos[0].x, pos[0].y }, { pos[1].x, pos[1].y }, { pos[2].x, pos[2].y }, { pos[3].x, pos[3].y }, color);
}

void Screen::DrawImage(ImTextureID tex_id, const Vector2D& min, const Vector2D& size, const Color& color)
{
	draw_list->AddImage(tex_id, { min.x, min.y }, { min.x + size.x, min.y + size.y }, { 0.0f, 0.0f }, { 1.f, 1.f }, color);
}

Vector2D Screen::GetTextSize(const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);

	auto ret = ImGui::CalcTextSize(buf);
	return Vector2D(ret.x, ret.y);
}

void Screen::DrawCircle(const Vector2D& pos, float rad, float step, const Color& color)
{
	draw_list->AddCircle({ pos.x, pos.y }, rad, color, step);
}

void Screen::DrawCircleFilled(const Vector2D& pos, float rad, float step, const Color& color)
{
	draw_list->AddCircleFilled({ pos.x, pos.y }, rad, color, step);
}

Vector2D Screen::GetCursorPos()
{
	return Vector2D(ImGui::GetCursorScreenPos().x, ImGui::GetCursorPos().y);
}

void Screen::DrawLineGlow(const Vector2D& min, const Vector2D& max, const Color& color, const Color& glowColor, const size_t glowLen)
{
	auto c0 = glowColor; c0.a() = 0;
	auto c255 = color; c255.a() = 1.0f;

	Rect led = { min.x, min.y, min.x + max.x, min.y + 3 };

	Rect glow = { led.left + glowLen, led.top - glowLen, led.right - glowLen, led.bot };
	Screen::DrawRectGradient(glow, c0, c0, c255, c255);

	glow.top = led.bot;
	glow.bot += glowLen;
	Screen::DrawRectGradient(glow, c255, c255, c0, c0);

	//glow.top = 

	Screen::DrawRectFilled(led, color);
}