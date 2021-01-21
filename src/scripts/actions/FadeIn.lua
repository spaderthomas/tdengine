local FadeIn = tdengine.action('FadeIn')

function FadeIn:init(params)
   self.time = params.time
end

function FadeIn:update(dt)
   self.done = true
end

tdengine.actions.FadeIn = FadeIn
