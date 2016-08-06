open Js.Unsafe;

let consolelog str => ignore @@ meth_call Firebug.console "log" [|inject (Js.string str)|];

let pixi = variable "PIXI";

let createContainer () => new_obj (get pixi "Container") [||];

let loadTexture callback::cb name::name uri::uri => {
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
  let on (element: any) evt (cb: any => unit) =>
    ignore @@
      meth_call
        element
        "on"
        [|inject (Js.string (stringForEventType evt)), inject (Js.wrap_meth_callback cb)|];
};

let module TwoDPoint = {
  class t x y => {
    as self;
    val mutable x = x;
    val mutable y = y;
  };
};

let module View = {
  class t = {
    as self;
  };
};

let module Texture = {
  class t uri => {
    as self;
    val _innerSelf: any = meth_call (get pixi "Texture") "fromImage" [|inject (Js.string uri)|];
    method raw = _innerSelf;
  };
  let fromImage uri::uri => (new t) uri;
};
let module Sprite = {
  class t texture => {
    as self;
    val _innerSelf: any = new_obj (get pixi "Sprite") [|texture#raw|];
    method raw = _innerSelf;
    method setWidth (v: int) => set _innerSelf "width" v;
    method setHeight (v: int) => set _innerSelf "height" v;
    method buttonMode: bool = get _innerSelf "buttonMode";
    method setButtonMode (b: bool) => ignore @@ set _innerSelf "buttonMode" b;
    method setAnchor x y =>
      ignore @@
        meth_call
          (get _innerSelf "anchor")
          "set"
          [|inject (Js.number_of_float x), inject (Js.number_of_float y)|];
    method setPosition (x: int) (y: int) => {
      let p = get _innerSelf "position";
      set p "x" x;
      set p "y" y
    };
    method setScale x y =>
      ignore @@
        meth_call
          (get _innerSelf "scale")
          "set"
          [|inject (Js.number_of_float x), inject (Js.number_of_float y)|];
    method setRotation (v: float) => set _innerSelf "rotation" v;
    method setInteractive (i: bool) => set _innerSelf "interactive" i;
    method setTap (f: unit => unit) => set _innerSelf "tap" f;
    method setClick (f: unit => unit) => set _innerSelf "click" f;
    method isDown = Js.to_bool (get _innerSelf "isDown");
    method setIsDown (i: bool) => set _innerSelf "isDown" i;
    method setAlpha (a: float) => set _innerSelf "alpha" a;
    method setTexture (t: Texture.t) => set _innerSelf "texture" t#raw;
    method isOver = Js.to_bool (get _innerSelf "isOver");
    method setIsOver (i: bool) => set _innerSelf "isOver" i;
    method on evt (cb : t => unit) => {
      let innerCb _ => cb (self :> t);
      Events.on _innerSelf evt innerCb
    };
  };
  let fromImage uri::uri => (new t) (Texture.fromImage uri::uri);
};

let module Container = {
  class t = {
    as self;
    val _innerSelf: any = new_obj (get pixi "Container") [||];
    method raw = _innerSelf;
    method addChild (child: Sprite.t) => ignore @@ meth_call _innerSelf "addChild" [|child#raw|];
  };
};

let module Renderer = {
  class t width height renderedStr => {
    as self;
    val _innerSelf =
      new_obj
        (get (variable "PIXI") renderedStr)
        [|
          inject (Js.number_of_float (float_of_int width)),
          inject (Js.number_of_float (float_of_int height))
        |];
    method width = int_of_float (Js.float_of_number (get _innerSelf "width"));
    method height = int_of_float (Js.float_of_number (get _innerSelf "height"));
    method view: View.t = get _innerSelf "view";
    method render (stage: Container.t) => ignore @@ meth_call _innerSelf "render" [|stage#raw|];
  };
};

let autoDetectRenderer width::width height::height =>
  (new Renderer.t) width height "autoDetectRenderer";
