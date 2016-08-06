open Js.Unsafe;

let consolelog str => ignore @@ meth_call Firebug.console "log" [|inject (Js.string str)|];

let pi = 3.14159265358979312;

let pixi = variable "PIXI";

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

let module ScaleModes = {type t = | Linear | Nearest;};

/* new PIXI.RenderTexture(renderer, 300, 200, PIXI.SCALE_MODES.LINEAR, 0.1); */
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
    method on evt (cb: t => unit) => {
      /* This is a "hack" to get the compiler to accept that the function innerCb is referencing
       * the class itself.
       * Look at http://caml.inria.fr/pub/docs/manual-ocaml/objectexamples.html#ss%3Ausing-coercions
       * and look for the example `class c = object (self) method m = (self :> c) end;;` which
       * is explained to be a special case for the compiler.*/
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
    method addChildContainer (child: t) =>
      ignore @@ meth_call _innerSelf "addChild" [|(child :> t)#raw|];
    method setPosition (x: int) (y: int) => {
      set _innerSelf "x" x;
      set _innerSelf "y" y
    };
  };
};

let module RenderTexture = {
  class t
        scaleModes::scaleModes=ScaleModes.Linear
        sampleRatio::sampleRatio=1.0
        (width: int)
        (height: int) => {
    as self;
    val _innerSelf: any =
      meth_call
        (get pixi "RenderTexture")
        "create"
        [|
          inject (Js.number_of_float (float_of_int width)),
          inject (Js.number_of_float (float_of_int height)),
          inject scaleModes,
          inject sampleRatio
        |];
    method raw = _innerSelf;
    method render (c: Container.t) => ignore @@ meth_call _innerSelf "render" [|c#raw|];
  };
};

type paramT = {backgroundColor: int};

let paramToObj param => {
  let p = new_obj (get (variable "window") "Object") [||];
  set p "backgroundColor" param.backgroundColor;
  p
};

let module Renderer = {
  class t param::(param: option paramT)=? width height renderedStr => {
    as self;
    val _innerSelf: any =
      switch param {
      | None =>
        new_obj
          (get pixi renderedStr)
          [|
            inject (Js.number_of_float (float_of_int width)),
            inject (Js.number_of_float (float_of_int height))
          |]
      | Some param =>
        new_obj
          (get pixi renderedStr)
          [|
            inject (Js.number_of_float (float_of_int width)),
            inject (Js.number_of_float (float_of_int height)),
            inject (paramToObj param)
          |]
      };
    method raw = _innerSelf;
    method width = int_of_float (Js.float_of_number (get _innerSelf "width"));
    method height = int_of_float (Js.float_of_number (get _innerSelf "height"));
    method view: View.t = get _innerSelf "view";
    method render renderTexture::(renderTexture: option RenderTexture.t)=? (stage: Container.t) =>
      ignore @@
        meth_call
          _innerSelf
          "render"
          (
            switch renderTexture {
            | None => [|stage#raw|]
            | Some rt => [|stage#raw, rt#raw|]
            }
          );
    method renderToTexture (container: Container.t) (renderTexture: RenderTexture.t) =>
      ignore @@ meth_call _innerSelf "render" [|container#raw, renderTexture#raw|];
    method renderToSprite (renderTexture: RenderTexture.t) (sprite: Sprite.t) =>
      ignore @@ meth_call _innerSelf "render" [|sprite#raw, renderTexture#raw|];
  };
};

let module Dom = {
  let appendToBody (child: View.t) =>
    ignore @@ meth_call (get Dom_html.document "body") "appendChild" [|inject child|];
};

let autoDetectRenderer width::width height::height =>
  (new Renderer.t) width height "autoDetectRenderer";

let autoDetectRendererWithParam param::param width::width height::height =>
  (new Renderer.t) param::param width height "autoDetectRenderer";
