require("overseer").register_template({
  name = "Configure CMake",
  params = {},
  condition = {
    -- This makes the template only available in the current directory
    -- In case you :cd out later
    dir = vim.fn.getcwd(),
  },
  builder = function()
    return {
      cmd = "cmake",
      args = {"-Bbuild"},
    }
  end,
})

require("overseer").register_template({
  name = "Build networking",
  params = {},
  condition = {
    -- This makes the template only available in the current directory
    -- In case you :cd out later
    dir = vim.fn.getcwd(),
  },
  builder = function()
    return {
      cmd = "cmake",
      args = {"--build","build","--target","networking"},
    }
  end,
})
