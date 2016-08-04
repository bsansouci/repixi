open Js.Unsafe;

class t = {
  as self;
  val _innerSelf = new_obj (get (variable "PIXI") "Container") [||];
  method innerSelf : any = inject _innerSelf;
  method addChild (child : Renderable.t) => ignore @@ meth_call _innerSelf "addChild" [|inject child|];
};
