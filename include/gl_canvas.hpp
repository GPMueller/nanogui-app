#include <nanogui/canvas.h>
#include <nanogui/shader.h>

class GlCanvas : public nanogui::Canvas
{
public:
    GlCanvas( nanogui::Widget * parent );

    void set_rotation( float rotation );

    virtual void draw_contents();

private:
    nanogui::ref<nanogui::Shader> m_shader;
    float m_rotation;
};