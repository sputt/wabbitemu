using System;
using System.Collections.Generic;
using System.Drawing;

namespace Revsoft.MapEditor
{
    class Map
    {
        public Bitmap mapIcon;
        public int width;
        public int height;
        public int tileSize;
        private List<Bitmap> TileSet;
        private byte[,] tiles;

        public Map(int width, int height, int tileSize)
        {
            this.width = width;
            this.height = height;
            this.tileSize = tileSize;
            tiles = new byte[width, height];
            TileSet = new List<Bitmap>();
        }

        public bool addTile(Bitmap tile)
        {
            bool add = ContainsTile(tile);
            if(!add)
                TileSet.Add(tile);
            return add;
        }

        private bool ContainsTile(Bitmap tile)
        {
            foreach(Bitmap bit in TileSet)
                if (bit.Tag.ToString() == tile.Tag.ToString())
                    return true;
            return false;
        }

        public void removeTile(int tile)
        {
            TileSet.RemoveAt(tile);
        }

        public void setTile(int row, int column, byte tileNum)
        {
            tiles[row, column] = tileNum;
        }

        public void drawMap(Bitmap bitmap)
        {

        }

        public void generateIcon()
        {

        }
    }
}
