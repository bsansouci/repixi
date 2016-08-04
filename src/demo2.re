let start () => {
  let open Js.Unsafe;
  let renderer = Renderer.autoDetectRenderer width::800 height::600;
  ignore (meth_call (get Dom_html.document "body") "appendChild" [|inject renderer#view|]);
  let stage = new Container.t;
  let background = Repixi.createSpriteFromImage uri::"_assets/button_test_BG.jpg";
  set background "width" renderer#width;
  set background "height" renderer#height;
  stage#addChild background;
  let textureButton = Repixi.createTextureFromImage uri::"_assets/button.png";
  let textureButtonDown = Repixi.createTextureFromImage uri::"_assets/buttonDown.png";
  let textureButtonOver = Repixi.createTextureFromImage uri::"_assets/buttonOver.png";
  let buttons = ref [];
  let buttonPositions = [|175, 75, 655, 75, 410, 325, 150, 465, 685, 445|];
  let noop () => Repixi.consolelog "click";
  let onButtonDown this => {
    set this "isDown" true;
    set this "texture" textureButtonDown;
    set this "alpha" 1.0
  };
  let onButtonUp this => {
    set this "isDown" false;
    if (Js.to_bool (get this "isOver")) {
      set this "texture" textureButtonOver
    } else {
      set this "texture" textureButton
    }
  };
  let onButtonOver this => {
    set this "isOver" true;
    if (not (Js.to_bool (get this "isDown"))) {
      set this "texture" textureButtonOver
    }
  };
  let onButtonOut this => {
    set this "isOver" false;
    if (not (Js.to_bool (get this "isDown"))) {
      set this "texture" textureButton
    }
  };
  for i in 0 to 4 {
    Repixi.consolelog (string_of_int i);
    let button = Repixi.createSprite texture::textureButton;
    set button "buttonMode" true;
    ignore @@ meth_call (get button "anchor") "set" [|inject (Js.number_of_float 0.5)|];
    set (get button "position") "x" buttonPositions.(i * 2);
    set (get button "position") "y" buttonPositions.(i * 2 + 1);
    set button "interactive" true;
    {
      let open Repixi.Events;
      on button MouseDown onButtonDown;
      on button TouchStart onButtonDown;
      on button MouseUp onButtonUp;
      on button TouchEnd onButtonUp;
      on button MouseUpOutside onButtonUp;
      on button TouchEndOutside onButtonUp;
      on button MouseOver onButtonOver;
      on button MouseOut onButtonOut;
    };
    set button "tap" noop;
    set button "click" noop;
    stage#addChild button;
    buttons := !buttons @ [button]
  };
  ignore @@ meth_call (get (List.nth !buttons 0) "scale") "set" [|inject (Js.number_of_float 1.2)|];
  set (List.nth !buttons 2) "rotation" (3.14159265358979312 /. 10.);
  ignore @@ meth_call (get (List.nth !buttons 3) "scale") "set" [|inject (Js.number_of_float 0.8)|];
  ignore @@
    meth_call
      (get (List.nth !buttons 4) "scale")
      "set"
      [|inject (Js.number_of_float 0.8), inject (Js.number_of_float 1.2)|];
  set (List.nth !buttons 4) "rotation" 3.14159265358979312;
  let rec animate () => {
    let open Js.Unsafe;
    Dom_html._requestAnimationFrame (Js.wrap_callback animate);
    renderer#render stage;
  };
  animate ()
};
