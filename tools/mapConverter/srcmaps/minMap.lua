return {
  version = "1.1",
  luaversion = "5.1",
  tiledversion = "0.15.1",
  orientation = "orthogonal",
  renderorder = "right-down",
  width = 10,
  height = 5,
  tilewidth = 32,
  tileheight = 32,
  nextobjectid = 1,
  properties = {},
  tilesets = {
    {
      name = "minMap",
      firstgid = 1,
      tilewidth = 32,
      tileheight = 32,
      spacing = 0,
      margin = 0,
      image = "minMap.png",
      imagewidth = 448,
      imageheight = 192,
      tileoffset = {
        x = 0,
        y = 0
      },
      properties = {},
      terrains = {},
      tilecount = 84,
      tiles = {
        {
          id = 42,
          properties = {
            ["ground"] = "Wall"
          }
        },
        {
          id = 43,
          properties = {
            ["ground"] = "Wall"
          }
        },
        {
          id = 44,
          properties = {
            ["ground"] = "Wall"
          }
        }
      }
    }
  },
  layers = {
    {
      type = "tilelayer",
      name = "bottom0",
      x = 0,
      y = 0,
      width = 10,
      height = 5,
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      properties = {
        ["layer"] = "0"
      },
      encoding = "lua",
      data = {
        0, 6, 7, 7, 7, 7, 7, 7, 8, 0,
        0, 20, 44, 43, 45, 43, 21, 45, 22, 0,
        15, 38, 21, 21, 21, 21, 21, 21, 40, 16,
        0, 20, 21, 21, 21, 21, 21, 21, 22, 0,
        0, 34, 35, 35, 35, 35, 35, 35, 36, 0
      }
    },
    {
      type = "tilelayer",
      name = "middle0",
      x = 0,
      y = 0,
      width = 10,
      height = 5,
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      properties = {
        ["layer"] = "1"
      },
      encoding = "lua",
      data = {
        0, 0, 31, 31, 31, 31, 0, 31, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
      }
    }
  }
}
