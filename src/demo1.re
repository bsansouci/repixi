let module Repixi = {
  open Js.Unsafe;
  let consolelog str => ignore @@ meth_call Firebug.console "log" [|inject (Js.string str)|];
  let pixi = variable "PIXI";
  let createContainer () => new_obj (get pixi "Container") [||];
  let loadTexture callback::cb name::name uri::uri => {
    let open Js.Unsafe;
    let loader =
      meth_call (get pixi "loader") "add" [|inject (Js.string name), inject (Js.string uri)|];
    meth_call loader "load" [|inject (Js.wrap_callback cb)|]
  };
  let createSprite texture::texture => new_obj (get pixi "Sprite") [|texture|];
  let createSpriteFromImage uri::uri =>
    meth_call (get pixi "Sprite") "fromImage" [|inject (Js.string uri)|];
  let addToStage stage::stage obj => ignore @@ meth_call stage "addChild" [|inject obj|];
  let createTextureFromImage uri::uri =>
    meth_call (get pixi "Texture") "fromImage" [|inject (Js.string uri)|];
  let module Events = {
    type t;
    type eventT =
      | MouseDown
      | MouseUp
      | MouseUpOutside
      | MouseOver
      | MouseOut
      | TouchStart
      | TouchEnd
      | TouchEndOutside;
    let stringForEventType evtType =>
      switch evtType {
      | MouseDown => "mousedown"
      | MouseUp => "mouseup"
      | MouseUpOutside => "mouseupoutside"
      | MouseOver => "mouseover"
      | MouseOut => "mouseout"
      | TouchStart => "touchstart"
      | TouchEnd => "touchend"
      | TouchEndOutside => "touchendoutside"
      };
    let on element evt cb =>
      ignore @@
        meth_call
          element
          "on"
          [|inject (Js.string (stringForEventType evt)), inject (Js.wrap_meth_callback cb)|];
  };
};

let start () => {
  let open Js.Unsafe;
  let renderer =
    new_obj
      (get (variable "PIXI") "WebGLRenderer")
      [|
        inject (Js.number_of_float (float_of_int 800)),
        inject (Js.number_of_float (float_of_int 600))
      |];
  ignore (meth_call (get Dom_html.document "body") "appendChild" [|get renderer "view"|]);
  let stage = Repixi.createContainer ();
  Repixi.loadTexture
    callback::(
      fun loader resources => {
        let bunny = Repixi.createSprite texture::(get (get resources "bunny") "texture");
        set (get bunny "position") "x" 400;
        set (get bunny "position") "y" 300;
        set (get bunny "scale") "x" 2;
        set (get bunny "scale") "y" 2;
        ignore @@ meth_call stage "addChild" [|inject bunny|];
        let rec animate () => {
          Dom_html._requestAnimationFrame (Js.wrap_callback animate);
          set bunny "rotation" (Js.to_float (get bunny "rotation") +. 0.01);
          ignore @@ meth_call renderer "render" [|inject stage|]
        };
        animate ()
      }
    )
    name::"bunny"
    uri::"_assets/bunny.png"
};
