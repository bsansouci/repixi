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
