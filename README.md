# Google Instant Apps

This is [Native Extension](https://www.defold.com/manuals/extensions/) for the [Defold Game Engine](https://www.defold.com) implement [Google Play Instant](https://developer.android.com/topic/google-play-instant/overview) API methods.

[Here you can find](https://forum.defold.com/t/instruction-android-instant-app-creation/48471?u=agulev) instruction how to build instant app using Defold.

## Setup

You can use the InstantApp extension in your own project by adding this project as a [Defold library dependency](https://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

https://github.com/AGulev/instantapp/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/AGulev/instantapp/releases).

## API

If you are working on cross-platform application the best practice to check the existence of `instantapp` module, this module exists only in android bundle:
```lua
if instantapp then
  -- call instantapp methods
end
```

#### `instantapp.is_instant_app()`
Returns true if this application is an instant app.
```lua
if instantapp.is_instant_app() then
  --do something specific for instant app
end
```

#### `instantapp.show_install_prompt()`
Shows a dialog that allows the user to install the current instant app.

```lua
if instantapp.is_instant_app() then
  instantapp.show_install_prompt() -- if this is instant app then show install prompt
else
  -- regular app logic
end
```
Popup example:

![2019-04-07 20 54 02](https://user-images.githubusercontent.com/2209596/55688836-b2f9a500-597d-11e9-8618-c675415983f5.jpg)


## Issues and suggestions

If you have any issues, questions or suggestions please [create an issue](https://github.com/AGulev/instantapp/issues) or contact me: me@agulev.com
