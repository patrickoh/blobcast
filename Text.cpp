#include "Text.h"

Text::Text(VertexArray *vao, Font *font) :
	VAO(vao), FontStyle(font), Vertices(NULL), TexCoords(NULL) { }

Text::~Text()
{
	delete Vertices;
	delete TexCoords;
}

void Text::Draw() const
{
	glEnableVertexAttribArray(0);
	Vertices->BufferData(0);
	glEnableVertexAttribArray(1);
	TexCoords->BufferData(1);

	glDrawArrays(GL_TRIANGLES, 0, NumVerts);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Text::SetText(std::string text)
{
	float pen_x = XPosition;
	float pen_y = YPosition;
	delete Vertices;
	delete TexCoords;

	std::size_t length = text.size();
	NumVerts = length * 6;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> texcoords;

	for (std::size_t i = 0; i < length; i++)
	{
		texture_glyph_t *glyph = FontStyle->GetGlyph(&text[i]);
		if (glyph != NULL)
		{
			float kerning = 0.f;
			if (i > 0)
			{
				kerning = FontStyle->GetKerning(glyph, &text[i - 1]);
			}
			pen_x += kerning;
			float x0 = pen_x + glyph->offset_x;
			float y0 = pen_y + glyph->offset_y;
			float x1 = x0 + glyph->width;
			float y1 = y0 - glyph->height;
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;
			std::vector<GLfloat> verts({{
					x0, y0,
					x0, y1,
					x1, y1,
					x0, y0,
					x1, y1,
					x1, y0 }});
			std::vector<GLfloat> texcs({{
					s0, t0,
					s0, t1,
					s1, t1,
					s0, t0,
					s1, t1,
					s1, t0 }});
			vertices.insert(vertices.end(), verts.begin(), verts.end());
			texcoords.insert(texcoords.end(), texcs.begin(), texcs.end());
			pen_x += glyph->advance_x;
		}
	}

	Vertices = new FloatBuffer(VAO, 2, NumVerts);
	Vertices->SetData(&vertices[0]);
	TexCoords = new FloatBuffer(VAO, 2, NumVerts);
	TexCoords->SetData(&texcoords[0]);
}