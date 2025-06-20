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
      name = "Configure CMake",
      cmd = "cmake",
      args = {"-Bbuild"},
    }
  end,
})

require("overseer").register_template({
  name = "Build server",
  params = {},
  condition = {
    -- This makes the template only available in the current directory
    -- In case you :cd out later
    dir = vim.fn.getcwd(),
  },
  builder = function()
    return {
      name = "Build server",
      cmd = "cmake",
      args = {"--build","build","--target","server"},
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
      name = "Build networking",
      cmd = "cmake",
      args = {"--build","build","--target","networking"},
    }
  end,
})

require("overseer").register_template({
  name = "Build networking test",
  params = {},
  condition = {
    -- This makes the template only available in the current directory
    -- In case you :cd out later
    dir = vim.fn.getcwd(),
  },
  builder = function()
    return {
      name = "Build networking",
      cmd = "cmake",
      args = {"--build","build","--target","networking_test"},
    }
  end,
})
