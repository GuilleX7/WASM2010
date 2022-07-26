<template>
  <form action="">
    <div class="modal-card">
      <header class="modal-card-head">
        <p class="modal-card-title">Settings</p>
        <button type="button" class="delete" @click="$emit('close')" />
      </header>
      <section class="modal-card-body">
        <b-tabs v-model="activeTab" expanded>
          <b-tab-item label="Pictures">
            Lorem ipsum dolor sit amet.
          </b-tab-item>

          <b-tab-item label="Music">
            Lorem <br />
            ipsum <br />
            dolor <br />
            sit <br />
            amet.
          </b-tab-item>

          <b-tab-item label="Books">
            What light is light, if Silvia be not seen? <br />
            What joy is joy, if Silvia be not by— <br />
            Unless it be to think that she is by <br />
            And feed upon the shadow of perfection? <br />
            Except I be by Silvia in the night, <br />
            There is no music in the nightingale.
          </b-tab-item>

          <b-tab-item label="Videos">
            Nunc nec velit nec libero vestibulum eleifend. Curabitur pulvinar
            congue luctus. Nullam hendrerit iaculis augue vitae ornare. Maecenas
            vehicula pulvinar tellus, id sodales felis lobortis eget.
          </b-tab-item>
        </b-tabs>
      </section>
      <footer class="modal-card-foot">
        <b-button label="Close" @click="$emit('close')" />
        <b-button label="Save changes" type="is-primary" />
      </footer>
    </div>
  </form>
</template>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';

export default defineComponent({
  data: () => ({
    email: '',
    password: '',
    activeTab: undefined,
  }),
});
</script>
