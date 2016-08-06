let start () => {
  let open Js.Unsafe;
  let renderer = Repixi.autoDetectRenderer width::800 height::600;
  Repixi.Dom.appendToBody renderer#view;
  let stage = new Repixi.Container.t;
  let background = Repixi.Sprite.fromImage uri::"_assets/button_test_BG.jpg";
  background#setWidth renderer#width;
  background#setHeight renderer#height;
  stage#addChild background;
  let textureButton = Repixi.Texture.fromImage uri::"_assets/button.png";
  let textureButtonDown = Repixi.Texture.fromImage uri::"_assets/buttonDown.png";
  let textureButtonOver = Repixi.Texture.fromImage uri::"_assets/buttonOver.png";
  let buttons = ref [];
  let buttonPositions = [|175, 75, 655, 75, 410, 325, 150, 465, 685, 445|];
  let noop () => Repixi.consolelog "click";
  let onButtonDown this => {
    this#setIsDown true;
    this#setTexture textureButtonDown;
    this#setAlpha 1.0
  };
  let onButtonUp this => {
    this#setIsDown false;
    if this#isOver {
      this#setTexture textureButtonOver
    } else {
      this#setTexture textureButton
    }
  };
  let onButtonOver this => {
    this#setIsOver true;
    if (not this#isDown) {
      this#setTexture textureButtonOver
    }
  };
  let onButtonOut this => {
    this#setIsOver false;
    if (not this#isDown) {
      this#setTexture textureButton
    }
  };
  for i in 0 to 4 {
    let button = (new Repixi.Sprite.t) textureButton;
    button#setButtonMode true;
    button#setAnchor 0.5 0.5;
    button#setPosition buttonPositions.(i * 2) buttonPositions.(i * 2 + 1);
    button#setInteractive true;
    {
      let open Repixi.Events;
      button#on MouseDown onButtonDown;
      button#on TouchStart onButtonDown;
      button#on MouseUp onButtonUp;
      button#on TouchEnd onButtonUp;
      button#on MouseUpOutside onButtonUp;
      button#on TouchEndOutside onButtonUp;
      button#on MouseOver onButtonOver;
      button#on MouseOut onButtonOut
    };
    button#setTap noop;
    button#setClick noop;
    stage#addChild button;
    buttons := !buttons @ [button]
  };
  (List.nth !buttons 0)#setScale 1.2 1.2;
  (List.nth !buttons 2)#setRotation (3.14159265358979312 /. 10.);
  (List.nth !buttons 3)#setScale 0.8 0.8;
  (List.nth !buttons 4)#setScale 0.8 1.2;
  (List.nth !buttons 4)#setRotation 3.14159265358979312;
  let rec animate () => {
    Dom_html._requestAnimationFrame (Js.wrap_callback animate);
    renderer#render stage
  };
  animate ()
};
