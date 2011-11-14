#include "fitz.h"

static fz_obj *
fz_resolve_indirect_null(fz_obj *ref)
{
	return ref;
}

fz_obj *(*fz_resolve_indirect)(fz_obj*) = fz_resolve_indirect_null;

void
fz_free_context(fz_context *ctx)
{
	if (ctx == NULL)
		return;

	/* Other finalisation calls go here (in reverse order) */

	if (ctx->error)
	{
		assert(ctx->error->top == -1);
		fz_free(ctx, ctx->error);
	}

	/* Free the context itself */
	ctx->alloc->free(ctx->alloc->user, ctx);
}

fz_context *
fz_new_context(fz_alloc_context *alloc)
{
	fz_context *ctx;

	ctx = alloc->malloc(alloc->user, sizeof(fz_context));
	if (!ctx)
		return NULL;
	memset(ctx, 0, sizeof *ctx);
	ctx->alloc = alloc;

	ctx->error = fz_malloc_no_throw(ctx, sizeof(fz_error_context));
	if (!ctx->error)
		goto cleanup;
	ctx->error->top = -1;
	ctx->error->message[0] = 0;

	ctx->warn = fz_malloc_no_throw(ctx, sizeof(fz_warn_context));
	if (!ctx->warn)
		goto cleanup;
	ctx->warn->message[0] = 0;
	ctx->warn->count = 0;

	/* New initialisation calls for context entries go here */

	return ctx;

cleanup:
	fprintf(stderr, "cannot create context\n");
	fz_free_context(ctx);
	return NULL;
}

fz_context *
fz_clone_context(fz_context *ctx)
{
	return fz_new_context(ctx->alloc);
}