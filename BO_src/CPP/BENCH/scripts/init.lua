html = require "html"

function GetPerformancePage()
   return html.performance(
      html.script(),
      '["10:10", "10:15", "10:20", "10:25", "10:30", "10:35", "10:40"]',
      "[270, 420, 390, 590, 610, 320, 800]",
      "[280, 480, 400, 190, 860, 270, 900]",
      "[310, 230, 511, 427, 160, 880, 50]"
      )
end

return GetPerformancePage()
