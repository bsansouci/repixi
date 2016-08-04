open Js.Unsafe;
class t width height renderedStr => {
  as self;
  val _innerSelf = new_obj
      (get (variable "PIXI") renderedStr)
      [|
        inject (Js.number_of_float (float_of_int width)),
        inject (Js.number_of_float (float_of_int height))
      |];

  method width = Js.float_of_number (get _innerSelf "width");
  method height = Js.float_of_number (get _innerSelf "height");
  method view : View.t = get _innerSelf "view";
  method render (stage : Container.t) => ignore @@ meth_call _innerSelf "render" [|stage#innerSelf|];
};


let autoDetectRenderer width::width height::height => new t width height "autoDetectRenderer";
