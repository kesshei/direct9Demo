using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace _02_BasicPrimitive_winfrom
{
    public partial class Form1 : Form
    {
        // 提取顶点的 x、y 坐标（原数据中的前两个值）
        List<PointF> vertices = new List<PointF>
        {
            new PointF(50.0f, 250.0f),   // 顶点0
            new PointF(150.0f, 50.0f),   // 顶点1
            new PointF(250.0f, 250.0f),  // 顶点2
            new PointF(350.0f, 50.0f),   // 顶点3
            new PointF(450.0f, 250.0f),  // 顶点4
            new PointF(550.0f, 50.0f)    // 顶点5
        };
        public Form1()
        {
            InitializeComponent();
            // 设置 panel1 背景为白色，方便观察
            panel1.BackColor = Color.White;
            // 绑定 Paint 事件（每次重绘时调用）
            panel1.Paint += Panel1_Paint;
        }

        private void Panel1_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;

            // 1. 绘制三角形线框
            using (Pen trianglePen = new Pen(Color.Red, 2))
            {
                g.DrawPolygon(trianglePen, new[] { vertices[0], vertices[1], vertices[2] });
                g.DrawPolygon(trianglePen, new[] { vertices[3], vertices[4], vertices[5] });
            }

            // 2. 标记顶点（带序号的圆点）
            float dotSize = 8f;  // 顶点圆点大小
            using (SolidBrush dotBrush = new SolidBrush(Color.Red))
            using (SolidBrush indexBrush = new SolidBrush(Color.White))  // 序号背景色（白色，突出序号）
            using (Pen indexPen = new Pen(Color.Black, 1))  // 序号边框
            using (Font indexFont = new Font("Arial", 7f, FontStyle.Bold))  // 序号字体（加粗）
            {
                for (int i = 0; i < vertices.Count; i++)
                {
                    PointF p = vertices[i];
                    // 绘制顶点圆点（红色）
                    RectangleF dotRect = new RectangleF(
                        p.X - dotSize / 2,
                        p.Y - dotSize / 2,
                        dotSize,
                        dotSize
                    );
                    g.FillEllipse(dotBrush, dotRect);

                    // 在圆点中心绘制序号（如“0”“1”），白色背景+黑色边框
                    string indexText = i.ToString();
                    // 计算序号文本的位置（居中显示在圆点内）
                    SizeF textSize = g.MeasureString(indexText, indexFont);
                    PointF indexPos = new PointF(
                        p.X - textSize.Width / 2,
                        p.Y - textSize.Height / 2
                    );
                    // 绘制序号背景（白色小圆，覆盖红色圆点中心）
                    g.FillEllipse(indexBrush,
                        indexPos.X - 1, indexPos.Y - 1,
                        textSize.Width + 2, textSize.Height + 2);
                    // 绘制序号边框（黑色细线）
                    g.DrawEllipse(indexPen,
                        indexPos.X - 1, indexPos.Y - 1,
                        textSize.Width + 2, textSize.Height + 2);
                    // 绘制序号文本（黑色加粗）
                    g.DrawString(indexText, indexFont, Brushes.Black, indexPos);
                }
            }

            // 3. 标注坐标（明确显示“点i：(x, y)”）
            using (Font coordFont = new Font("微软雅黑", 9f))
            using (SolidBrush coordBrush = new SolidBrush(Color.DarkBlue))  // 坐标文本用深蓝色，区分序号
            {
                float offsetX = 10f;  // 文本X偏移（右移）
                float offsetY = -5f;  // 文本Y偏移（上移，避免与圆点重叠）
                for (int i = 0; i < vertices.Count; i++)
                {
                    PointF p = vertices[i];
                    // 文本格式：“点i：(x, y)”
                    string coordText = $"点{i}：({p.X:F1}, {p.Y:F1})";
                    // 文本位置：顶点坐标 + 偏移
                    PointF textPos = new PointF(p.X + offsetX, p.Y + offsetY);
                    g.DrawString(coordText, coordFont, coordBrush, textPos);
                }
            }
        }
        // 可选：窗口大小改变时重绘
        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            panel1.Invalidate();  // 触发重绘
        }
    }
}
