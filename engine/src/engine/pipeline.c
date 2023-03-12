#include "pipeline.h"

//ECS_TAG(InkPipeline);

ECS_ENTITY_DECLARE(InkOnInput);
ECS_ENTITY_DECLARE(InkOnUpdate);
ECS_ENTITY_DECLARE(InkOnPreRender);
ECS_ENTITY_DECLARE(InkOnRender);
ECS_ENTITY_DECLARE(InkOnPostRender);
ECS_ENTITY_DECLARE(InkOnStore);

/* ink_pipeline_init */
int
ink_pipeline_init(
  engine_t *eng
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnInput, EcsPhase);
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnUpdate, EcsPhase);
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnPreRender, EcsPhase);
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnRender, EcsPhase);
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnPostRender, EcsPhase);
  ECS_ENTITY_DEFINE(ecs_ctx, InkOnStore, EcsPhase);

  ecs_add_pair(ecs_ctx, InkOnUpdate, EcsDependsOn, InkOnInput);
  ecs_add_pair(ecs_ctx, InkOnPreRender, EcsDependsOn, InkOnUpdate);
  ecs_add_pair(ecs_ctx, InkOnRender, EcsDependsOn, InkOnPreRender);
  ecs_add_pair(ecs_ctx, InkOnPostRender, EcsDependsOn, InkOnRender);
  ecs_add_pair(ecs_ctx, InkOnStore, EcsDependsOn, InkOnPostRender);

  //ecs_entity_t ink_pipeline = ecs_pipeline(ecs_ctx, {
  //  .query.filter.terms = {
  //    { .id = EcsSystem },
  //  },
  //});
  ////ECS_PIPELINE(ecs_ctx, ink_pipeline, EcsSystem, InkOnInput, InkOnUpdate);
  //ecs_set_pipeline(ecs_ctx, ink_pipeline);
  return 0;
}
