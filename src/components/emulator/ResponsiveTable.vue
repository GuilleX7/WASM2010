<template>
  <div class="is-fullwidth is-fullheight asm--emulator-table">
    <table class="is-fullwidth is-fullheight">
      <slot />
    </table>
  </div>
</template>

<style lang="scss">
.asm--emulator-table {
  overflow: auto;

  table {
    transition: opacity 86ms ease-out;
    border-collapse: separate;
    margin-bottom: 0px;
    background-color: white;
    color: $dark;

    tr {
      th {
        position: sticky;
        top: 0;
        z-index: 2;
        font-weight: 600;
        padding: 0.25rem 0.5rem;
        border: 2px solid $grey-lighter;
        border-width: 0 0 2px;
        background-color: white;
        height: 1.5rem;
      }

      td {
        font-family: monospace;
        padding: 0.25rem 0.5rem;
        border: 1px solid $grey-lighter;
        border-width: 0 0 1px;
      }
    }
  }
}
</style>
