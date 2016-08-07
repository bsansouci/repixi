/*
 * vim: set ft=rust:
 * vim: set ft=reason:
 */

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

let module Renderable = {
  class virtual t = {
    as self;
    method virtual raw: any;
    method setAnchor (x: float, y: float) =>
      ignore @@ meth_call (get self#raw "anchor") "set" [|inject x, inject y|];
    method position: (int, int) = {
      let p = get self#raw "position";
      let x = get p "x";
      let y = get p "y";
      (x, y)
    };
    method setPosition (x: int, y: int) => {
      let p = get self#raw "position";
      set p "x" x;
      set p "y" y
    };
    method scale: (float, float) = {
      let s = get self#raw "scale";
      (get s "x", get s "y")
    };
    method setScale (x: float, y: float) =>
      ignore @@ meth_call (get self#raw "scale") "set" [|inject x, inject y|];
    method rotation: float = get self#raw "rotation";
    method setRotation (v: float) => set self#raw "rotation" v;
    method interactive: bool = get self#raw "interactive";
    method setInteractive (i: bool) => set self#raw "interactive" i;
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

let module ScaleModes = {type t = | Linear | Nearest;};

let module Sprite = {
  class t texture => {
    as self;
    inherit class Renderable.t;
    val _innerSelf: any = new_obj (get pixi "Sprite") [|texture#raw|];
    method raw = _innerSelf;
    method width: int = get _innerSelf "width";
    method height: int = get _innerSelf "height";
    method setWidth (v: int) => set _innerSelf "width" v;
    method setHeight (v: int) => set _innerSelf "height" v;
    method buttonMode: bool = get _innerSelf "buttonMode";
    method setButtonMode (b: bool) => ignore @@ set _innerSelf "buttonMode" b;
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

type spineT = {spineData: any};

let module Spine = {
  class t (spineData: spineT) => {
    as self;
    inherit class Renderable.t;
    val _innerSelf = new_obj (get (get pixi "spine") "Spine") [|spineData.spineData|];
    method raw = _innerSelf;
    /* No `too` isn't a typo, `to` is a keyword. */
    method setMixByName from::from too::too duration::(duration: float) =>
      ignore @@
        meth_call
          (get _innerSelf "stateData")
          "setMixByName"
          [|inject (Js.string from), inject (Js.string too), inject duration|];
    method setAnimationByName startTime::(startTime: int) animName::animName loop::(loop: bool) =>
      ignore @@
        meth_call
          (get _innerSelf "state")
          "setAnimationByName"
          [|inject startTime, inject (Js.string animName), inject loop|];
    method addAnimationByName
           startTime::(startTime: int)
           animName::animName
           loop::(loop: bool)
           randomInt::(randomInt: int) =>
      ignore @@
        meth_call
          (get _innerSelf "state")
          "addAnimationByName"
          [|inject startTime, inject (Js.string animName), inject loop, inject randomInt|];
  };
};

let module Container = {
  class t = {
    as self;
    inherit class Renderable.t;
    val _innerSelf: any = new_obj (get pixi "Container") [||];
    method raw = _innerSelf;
    method addChild (child: Sprite.t) => ignore @@ meth_call _innerSelf "addChild" [|child#raw|];
    method addChildContainer (child: t) =>
      ignore @@ meth_call _innerSelf "addChild" [|(child :> t)#raw|];
    method addChildSpine (child: Spine.t) =>
      ignore @@ meth_call _innerSelf "addChild" [|child#raw|];
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
        [|inject width, inject height, inject scaleModes, inject sampleRatio|];
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
      | None => new_obj (get pixi renderedStr) [|inject width, inject height|]
      | Some param =>
        new_obj (get pixi renderedStr) [|inject width, inject height, inject (paramToObj param)|]
      };
    method raw = _innerSelf;
    method width: int = get _innerSelf "width";
    method height: int = get _innerSelf "height";
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

let module Loader = {
  class t prevInner::(prevInner: option any)=? (name: string) (uri: string) => {
    as self;
    val _innerSelf: any =
      meth_call
        (
          switch prevInner {
          | None => get pixi "loader"
          | Some prevInner => prevInner
          }
        )
        "add"
        [|inject (Js.string name), inject (Js.string uri)|];
    method load (cb: t => spineT => unit) => {
      /* hack again... Now we're using `name` inside res to get the data itself...
       * I need to figure out what the best way to convert the "chain" paradigm that looks like
       * `obj.add().add().add().load()` where `load` changes the state of the whole thing
       * such that whenever any `add` is ready, it'll call the cb given to `load`.
       * I'm not sure that paradigm makes any sense here in OCaml. */
      let innerCb loader res => {
        let data = get res name;
        cb (self :> t) {spineData: get data "spineData"}
      };
      ignore @@ meth_call _innerSelf "load" [|inject (Js.wrap_callback innerCb)|]
    };
    method add name::name uri::uri => (new t) prevInner::_innerSelf name uri;
  };
  let add name::name uri::uri => (new t) name uri;
};

let module Dom = {
  let appendToBody (child: View.t) =>
    ignore @@ meth_call (get Dom_html.document "body") "appendChild" [|inject child|];
};

let autoDetectRenderer width::width height::height =>
  (new Renderer.t) width height "autoDetectRenderer";

let autoDetectRendererWithParam param::param width::width height::height =>
  (new Renderer.t) param::param width height "autoDetectRenderer";
