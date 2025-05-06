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
      args = {"-Bbuild","--build","--target","networking"},
    }
  end,
})
