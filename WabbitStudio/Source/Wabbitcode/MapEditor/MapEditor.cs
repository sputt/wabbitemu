using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.MapEditor
{
    public partial class MapEditor : Form
    {
        private int ratio = 2;
        private Map currentMap;
        public MapEditor()
        {
            InitializeComponent();
            currentMap = new Map(10, 10, 16);
        }

        private void switchMap(Map newMap)
        {
            tileManager.Controls.Clear();
            Bitmap bitmap = new Bitmap(newMap.width * ratio, newMap.height * ratio);
            map.Image = bitmap;
            newMap.drawMap(bitmap);
            currentMap = newMap;
        }

        private void addNewTile(Image tile)
        {

            PictureBox newTileBox = new PictureBox
                                        {
                                            Image = resizeImage(tile, tile.Size.Width*ratio, tile.Size.Height*ratio),
                                            Size = new Size(tile.Size.Width*ratio, tile.Size.Height*ratio),
                                            AllowDrop = true
                                        };
            tileManager.Controls.Add(newTileBox);
        }

        private void addExistMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() != DialogResult.OK) 
                return;
            FileStream reader = null;
            foreach (string file in openFileDialog.FileNames)
                try
                {
                    reader = new FileStream(file, FileMode.Open);
                    Bitmap bitmap = new Bitmap(reader);
                    bitmap.Tag = file;
                    bool add = currentMap.addTile(bitmap);
                    if (!add)
                        addNewTile(bitmap);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
                finally
                {
                    if (reader != null) 
                        reader.Close();
                }
        }

        private static Bitmap resizeImage(Image img, int width, int height)
        {
            Bitmap resized = new Bitmap(width + 1, height + 1);
            Graphics g = Graphics.FromImage(resized);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.DrawImage(img, 0, 0, width + 1, height + 1);
            g.Dispose();

            return resized;
        }
    }
}
