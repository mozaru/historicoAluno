// Pequeno cliente para a API do servidor C++
// Endpoints (da implementação fornecida):
// GET    /api/disciplinas
// GET    /api/disciplinas/{id}
// POST   /api/disciplinas
// PUT    /api/disciplinas/{id}
// DELETE /api/disciplinas/{id}
// GET    /api/cr

const $ = (sel, root=document) => root.querySelector(sel);
const $$ = (sel, root=document) => Array.from(root.querySelectorAll(sel));

const state = {
  data: [],
  filtered: [],
  selectedIndex: 0,
  filtro: ""
};

// ======= Modal infra (focus-trap e lock da Home) =======
class Modal {
  constructor(title, bodyNode, buttons=[]) {
    this.host = $("#modalHost");
    this.tpl = $("#tplDialogBase").content.cloneNode(true);
    this.backdrop = this.tpl.querySelector("[data-modal-backdrop]");
    this.modal = this.tpl.querySelector(".modal");
    this.titleEl = this.tpl.getElementById("modalTitle");
    this.bodyEl = this.tpl.getElementById("modalBody");
    this.footerEl = this.tpl.getElementById("modalFooter");

    this.titleEl.textContent = title;
    this.bodyEl.appendChild(bodyNode);
    buttons.forEach(b => this.footerEl.appendChild(b));

    this.onKeydown = this.onKeydown.bind(this);
  }
  open() {
    this.host.innerHTML = "";
    this.host.appendChild(this.tpl);
    this.host.classList.add("active");
    document.body.style.overflow = "hidden";
    document.addEventListener("keydown", this.onKeydown);
    // foco no primeiro input/botão
    setTimeout(() => {
      const focusable = this.host.querySelectorAll("button, [href], input, select, textarea, [tabindex]:not([tabindex='-1'])");
      if (focusable.length) focusable[0].focus();
    }, 0);
  }
  close() {
    this.host.classList.remove("active");
    this.host.innerHTML = "";
    document.body.style.overflow = "";
    document.removeEventListener("keydown", this.onKeydown);
    $("#filtro").focus();
  }
  onKeydown(e){
    if (e.key === "Escape") this.close();
    if (e.key === "Tab") {
      // focus trap simples
      const focusable = this.host.querySelectorAll("button, [href], input, select, textarea, [tabindex]:not([tabindex='-1'])");
      if (!focusable.length) return;
      const first = focusable[0], last = focusable[focusable.length - 1];
      if (e.shiftKey && document.activeElement === first){ last.focus(); e.preventDefault(); }
      else if (!e.shiftKey && document.activeElement === last){ first.focus(); e.preventDefault(); }
    }
  }
}

// ======= UI helpers =======
function button(label, cls, onClick){
  const b = document.createElement("button");
  b.className = `btn ${cls||""}`.trim();
  b.textContent = label;
  b.addEventListener("click", onClick);
  return b;
}

function toastDialog(tipo, mensagem, onOk){
  const body = document.createElement("div");
  body.innerHTML = `<p>${mensagem}</p>`;
  const ok = button("OK", "primary", () => { modal.close(); if (onOk) onOk(); });
  const modal = new Modal(tipo === "erro" ? "ERRO" : "SUCESSO", body, [ok]);
  modal.open();
}

// ======= API =======
async function apiGet(path){
  const r = await fetch(path, { headers: { "Accept": "application/json" } });
  if (!r.ok) throw await parseApiError(r);
  return r.json();
}
async function apiSend(method, path, data){
  const r = await fetch(path, {
    method,
    headers: { "Content-Type": "application/json", "Accept": "application/json" },
    body: data ? JSON.stringify(data) : undefined
  });
  if (!r.ok) throw await parseApiError(r);
  return r.status === 204 ? null : r.json();
}
async function parseApiError(r){
  let msg = "Ocorreu um erro inesperado.";
  try {
    const j = await r.json();
    if (j && typeof j.error === "string"){
      if (j.error === "infra" || j.error === "unexpected") msg = "Falha de infraestrutura. Tente novamente.";
      else msg = j.error; // BusinessError vem do servidor
    }
  } catch {}
  return new Error(msg);
}

// ======= Carregar dados =======
async function loadAll(){
  const arr = await apiGet("/api/disciplinas");
  state.data = Array.isArray(arr) ? arr : [];
  applyFilter();
  // CR
  try{
    const { cr } = await apiGet("/api/cr");
    $("#crPill").textContent = `CR: ${Number(cr).toFixed(2)}`;
  }catch{ /* opcional */ }
}

function applyFilter(){
  const f = state.filtro.trim().toLowerCase();
  if (!f) state.filtered = state.data.slice();
  else {
    state.filtered = state.data.filter(d => {
      const alvo = `${d.nome} ${d.matricula} ${d.ano}/${d.semestre}`.toLowerCase();
      return alvo.includes(f);
    });
  }
  state.selectedIndex = 0;
  renderTable();
}

// ======= Render =======
function renderTable(){
  const tb = $("#gridBody");
  tb.innerHTML = "";
  state.filtered.forEach((d, idx) => {
    const tr = document.createElement("tr");
    tr.setAttribute("role", "row");
    tr.setAttribute("aria-selected", idx === state.selectedIndex ? "true" : "false");
    tr.innerHTML = `
      <td scope="col" class="col-id">${d.id ?? ""}</td>
      <td scope="col" class="col-matricula">${d.matricula ?? ""}</td>
      <td scope="col" class="col-nome">${d.nome ?? ""}</td>
      <td scope="col" class="col-creditos">${d.creditos ?? ""}</td>
      <td scope="col" class="col-ano">${d.ano ?? ""}</td>
      <td scope="col" class="col-semestre">${d.semestre ?? ""}</td>
      <td scope="col" class="col-media">${(d.media ?? 0).toFixed(2)}</td>
      <td scope="col" class="col-acoes">
        <div class="action-group">
          <button class="btn edit" role="button" tabindex="0">Editar</button>
          <button class="btn remove" role="button" tabindex="0">Remover</button>
        </div>
      </td>
    `;
    tr.addEventListener("click", () => selectRow(idx));
    tr.addEventListener("dblclick", () => openEdit(d));
    tr.querySelector(".edit").addEventListener("click", (e)=>{ e.stopPropagation(); openEdit(d); });
    tr.querySelector(".remove").addEventListener("click", (e)=>{ e.stopPropagation(); openRemove(d); });
    tb.appendChild(tr);
  });
}

function selectRow(idx){
  state.selectedIndex = Math.max(0, Math.min(idx, state.filtered.length-1));
  $$("#gridBody tr").forEach((tr,i)=>tr.setAttribute("aria-selected", i===state.selectedIndex ? "true":"false"));
}

// ======= Dialogs =======
function openInsert(){
  const formNode = $("#tplFormDisciplina").content.cloneNode(true);
  const body = document.createElement("div");
  body.appendChild(formNode);
  const ok = button("OK", "success", async () => {
    const data = readForm($("#formDisciplina"));
    try{
      await apiSend("POST", "/api/disciplinas", data);
      modal.close();
      toastDialog("sucesso", "Disciplina inserida com sucesso.", () => loadAll());
    }catch(e){ modal.close(); toastDialog("erro", e.message, () => {}); }
  });
  const cancel = button("Cancelar", "ghost", () => modal.close());
  const modal = new Modal("DISCIPLINA", body, [cancel, ok]);
  modal.open();
}

function openEdit(d){
  const formNode = $("#tplFormDisciplina").content.cloneNode(true);
  const form = formNode.querySelector("form");
  setFormValues(form, d);
  const body = document.createElement("div");
  body.appendChild(formNode);
  const ok = button("OK", "primary", async () => {
    const data = readForm($("#formDisciplina"));
    try{
      await apiSend("PUT", `/api/disciplinas/${d.id}`, data);
      modal.close();
      toastDialog("sucesso", "Disciplina atualizada com sucesso.", () => loadAll());
    }catch(e){ modal.close(); toastDialog("erro", e.message, () => {}); }
  });
  const cancel = button("Cancelar", "ghost", () => modal.close());
  const modal = new Modal("DISCIPLINA", body, [cancel, ok]);
  modal.open();
}

function openRemove(d){
  const body = document.createElement("div");
  body.innerHTML = `
    <div class="confirm">
      <p><strong>ID:</strong> ${d.id}</p>
      <p><strong>Nome:</strong> ${d.nome}</p>
      <p><strong>Matrícula:</strong> ${d.matricula}</p>
      <p><strong>Ano/Sem:</strong> ${d.ano} / ${d.semestre}</p>
      <p><strong>Média:</strong> ${(d.media ?? 0).toFixed(2)}</p>
      <p style="margin-top:10px">Deseja realmente remover esta disciplina?</p>
    </div>
  `;
  const sim = button("Sim", "danger", async () => {
    try{
      await apiSend("DELETE", `/api/disciplinas/${d.id}`);
      modal.close();
      toastDialog("sucesso", "Disciplina removida com sucesso.", () => loadAll());
    }catch(e){ modal.close(); toastDialog("erro", e.message, () => {}); }
  });
  const nao = button("Não", "ghost", () => modal.close());
  const modal = new Modal("REMOVER DISCIPLINA", body, [nao, sim]);
  modal.open();
}

// helpers de formulário
function readForm(form){
  const fd = new FormData(form);
  const obj = Object.fromEntries(fd.entries());
  // normalizar tipos numéricos
  obj.creditos = Number(obj.creditos);
  obj.ano = Number(obj.ano);
  obj.semestre = Number(obj.semestre);
  obj.nota1 = Number(obj.nota1);
  obj.nota2 = Number(obj.nota2);
  return obj;
}
function setFormValues(form, d){
  form.querySelector('[name="nome"]').value = d.nome ?? "";
  form.querySelector('[name="matricula"]').value = d.matricula ?? "";
  form.querySelector('[name="creditos"]').value = d.creditos ?? 0;
  form.querySelector('[name="ano"]').value = d.ano ?? 0;
  form.querySelector('[name="semestre"]').value = d.semestre ?? 1;
  form.querySelector('[name="nota1"]').value = d.nota1 ?? 0;
  form.querySelector('[name="nota2"]').value = d.nota2 ?? 0;
}

// ======= Eventos da Home =======
$("#btnFiltrar").addEventListener("click", () => {
  state.filtro = $("#filtro").value;
  applyFilter();
});
$("#filtro").addEventListener("keydown", (e) => {
  if (e.key === "Enter"){ state.filtro = e.currentTarget.value; applyFilter(); }
});
$("#btnInserir").addEventListener("click", openInsert);

// navegação por teclado
$("#gridBody").addEventListener("keydown", (e) => {
  if (!state.filtered.length) return;
  if (e.key === "ArrowDown"){ selectRow(state.selectedIndex + 1); ensureRowVisible(); e.preventDefault(); }
  else if (e.key === "ArrowUp"){ selectRow(state.selectedIndex - 1); ensureRowVisible(); e.preventDefault(); }
  else if (e.key === "PageDown"){ scrollPage(1); e.preventDefault(); }
  else if (e.key === "PageUp"){ scrollPage(-1); e.preventDefault(); }
  else if (e.key === "Enter"){ const d = state.filtered[state.selectedIndex]; openEdit(d); e.preventDefault(); }
  else if (e.key === "Delete"){ const d = state.filtered[state.selectedIndex]; openRemove(d); e.preventDefault(); }
});
// permitir foco no tbody para hotkeys
$("#gridBody").tabIndex = 0;

function ensureRowVisible(){
  const container = $("#gridBody");
  const tr = container.children[state.selectedIndex];
  if (!tr) return;
  const rTop = tr.offsetTop;
  const rBottom = rTop + tr.offsetHeight;
  const vTop = container.scrollTop;
  const vBottom = vTop + container.clientHeight;
  if (rTop < vTop) container.scrollTop = rTop;
  else if (rBottom > vBottom) container.scrollTop = rBottom - container.clientHeight;
}

function scrollPage(dir){
  const container = $("#gridBody");
  container.scrollTop += dir * container.clientHeight * 0.9;
  const approxIndex = Math.floor(container.scrollTop / (container.scrollHeight / Math.max(state.filtered.length,1)) );
  selectRow(approxIndex);
}

// ======= Init =======
loadAll().catch(err => {
  toastDialog("erro", err.message);
});