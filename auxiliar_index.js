function coordenadas_elemento(obj) {
    return [ Number(obj.getAttribute("x")), Number(obj.getAttribute("y")) ];
}

function obtenerLlamada(ins){
    const matchFuncion = ins.match(/LLAMA\s+[-\d]+:[-\d]+\s+(\w+)/);
    const nombreFuncion = matchFuncion[1];
    const regexVariables = /(\w+)#(\d+)\s*=\s*([+-]?\d+)/g;
    let match, variables = [];
    while ((match = regexVariables.exec(ins)) !== null) {
        const nombre = match[1];
        const id_variable = match[2];
        const valor = match[3];
        variables.push({id_variable, nombre, valor});
    }
    return {nombreFuncion, variables};
}

function obtenerDeclaracion(ins) {
    const regex = /^DECLARA\s+\d+:\d+\s+([A-Za-z_]\w*)#(\d+)=([+-]?\d+)$/;
    const match = regex.exec(ins);
    if (!match) {
        throw new Error(`Formato de declaración inválido: "${ins}"`);
    }
    const [, nombre, id_variable, valor] = match;
    return { id_variable, nombre, valor };
}

function obtenerDestructor(ins) {
    const regex = /^DESTRUYE\s+\d+:\d+\s+([A-Za-z_]\w*)#(\d+)$/;
    const match = regex.exec(ins);
    if (!match) {
        throw new Error(`Formato de destructor inválido: "${ins}"`);
    }
    const [, nombre, id_variable] = match;
    return id_variable;
}

function obtenerRetorno(ins){
    const matchFuncion = ins.match(/REGRESA\s+\d+:\d+\s+([+-]?\d+)/);
    if (!matchFuncion) {
        throw new Error(`Formato de declaración inválido: "${ins}"`);
    }
    return matchFuncion[1];
}

function elimina_elemento(obj) {
    obj?.remove();
}


class NodoFuncion {
    contenido;
    variables;
    fila;
    columna;
    hijos;
    marco;
    texto;
    flecha;
    textoRetorno;
    constructor(variables) {
        this.hijos = [];
        this.variables = variables;
        this.fila = null;
        this.columna = null;
    }
}


class DibujoFuncion{
    nombreFuncion;
    flecha;
    constructor(marco){
        this.marco = marco;
        this.variables = new Map();
    }
    agregarVariable(id, nombre, valor, marco){
        this.variables.set(id, {nombre, valor, marco});
    }
    agregarNombreFuncion(nombreFuncion){
        this.nombreFuncion = nombreFuncion;
    }
    eliminarVariable(id){
        let variable = this.variables.get(id);
        elimina_elemento(variable.nombre);
        elimina_elemento(variable.valor);
        elimina_elemento(variable.marco);
        this.variables.delete(id);
    }
    obtenerCoordenadasMarco(){
        return coordenadas_elemento(this.marco);
    }
    destruirFuncion(){
        elimina_elemento(this.marco);
        elimina_elemento(this.nombreFuncion);
        for (const [clave, obj] of this.variables) {
            elimina_elemento(obj.nombre);
            elimina_elemento(obj.valor);
            elimina_elemento(obj.marco);
        }
        if(this.flecha !== null){
            elimina_elemento(this.flecha);
        }
    }
    obtenerCoordenadasUltimaVariable() {
        const vars = Array.from(this.variables.values());
        if (vars.length === 0) {
            return null; 
        }
        const ultimaVar = vars[vars.length - 1];
        return coordenadas_elemento(ultimaVar.marco);
    }
    obtenerMarco(){
        return this.marco;
    }
}

class ContrainstruccionArbol{
    accion;
    funcion;
    constructor(accion, funcion){
        this.accion = accion;
        this.funcion = funcion;
    }
}

class ContrainstruccionPila{
    accion;
    elemento;
    id_variable;
    marco_variable;
    constructor(accion, elemento){
        this.accion = accion;
        this.elemento = elemento;
    }
    agregarIdVariable(id_variable){
        this.id_variable = id_variable;
    }
    agregarMarcoVariable(marco_variable){
        this.marco_variable = marco_variable;
    }
}

class MarcoVariable{
    id;
    x;
    y;
    x_nombre;
    y_nombre;
    cont_nombre;
    x_valor;
    y_valor;
    cont_valor;
    constructor(id, x, y, x_nombre, y_nombre, cont_nombre, x_valor, y_valor, cont_valor){
        this.id = id;
        this.x = x;
        this.y = y;
        this.x_nombre = x_nombre;
        this.y_nombre = y_nombre;
        this.cont_nombre = cont_nombre;
        this.x_valor = x_valor;
        this.y_valor = y_valor;
        this.cont_valor = cont_valor;
    }
}

class MarcoFuncion{
    x;
    y;
    nombre_funcion;
    variables;
    constructor(x, y, nombre_funcion){
        this.x = x;
        this.y = y;
        this.nombre_funcion = nombre_funcion;
        this.variables = [];
    }
    agregarVariable(marco_variable){
        this.variables.push(marco_variable);
    }
}